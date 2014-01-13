#include "server/billing/main/servermanager.h"
#include "server/billing/connection/pool.h"
#include "server/common/base/config.h"
#include "server/common/base/log.h"
#include "server/common/base/time_manager.h"
#include "server/common/game/define/status/all.h"
#include "common/base/util.h"

const uint8_t kOneStepAccept = 50;

ServerManager* g_servermanager = NULL;

ServerManager::ServerManager() {
  __ENTER_FUNCTION
    FD_ZERO(&readfds_[kSelectFull]);
    FD_ZERO(&writefds_[kSelectFull]);
    FD_ZERO(&exceptfds_[kSelectFull]);
    maxfd_ = minfd_ = SOCKET_INVALID;
    fdsize_ = 0;
    setactive(true);
  __LEAVE_FUNCTION
}

ServerManager::~ServerManager() {
  __ENTER_FUNCTION
    SAFE_DELETE(serversocket_);
  __LEAVE_FUNCTION
}

bool ServerManager::init() {
  __ENTER_FUNCTION
    serversocket_ = new pap_server_common_net::Socket(g_config.billing_info_.port_);
    Assert(serversocket_);
    serversocket_->set_nonblocking();
    socketid_ = serversocket_->getid();
    Assert(socketid_ != SOCKET_INVALID);
    FD_SET(socketid_, &readfds_[kSelectFull]);
    FD_SET(socketid_, &exceptfds_[kSelectFull]);
    minfd_ = minfd_ = socketid_;
    timeout_[kSelectFull].tv_sec = 0;
    timeout_[kSelectFull].tv_usec = 0;
    threadid_ = pap_common_sys::get_current_thread_id();
    uint16_t i;
    for (i = 0; i < OVER_SERVER_MAX; ++i) {
      serverhash_[i] = ID_INVALID;
    }
  __LEAVE_FUNCTION
    return false;
}

bool ServerManager::select() {
  __ENTER_FUNCTION
    timeout_[kSelectUse].tv_sec = timeout_[kSelectFull].tv_sec;
    timeout_[kSelectUse].tv_usec = timeout_[kSelectFull].tv_usec;
    readfds_[kSelectUse] = readfds_[kSelectFull];
    writefds_[kSelectUse] = writefds_[kSelectFull];
    exceptfds_[kSelectUse] = exceptfds_[kSelectFull];
    pap_common_base::sleep(100);
    int32_t result = SOCKET_ERROR;
    try {
      result = pap_common_net::socket::base::select(
          maxfd_ + 1,
          &readfds_[kSelectUse],
          &writefds_[kSelectUse],
          &exceptfds_[kSelectUse]);
      Assert(result != SOCKET_ERROR);
    }
    catch(...) {
      g_log->fast_save_log(kBillingLogFile, 
                           "ServerManager::select have error, result: %d", 
                           result);
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ServerManager::processinput() {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == minfd_ && SOCKET_INVALID == maxfd_)
      return true; //no connection
    uint16_t i;
    if (FD_ISSET(socketid_, &readfds_[kSelectUse])) {
      for (i = 0; i < kOneStepAccept; ++i) {
        !accept_newconnection() && break;
      }
    }
    uint16_t connectioncount = connection::Manager::getcount();
    for (i = 0; i < connectioncount; ++i) {
      ID_INVALID == connectionids_[i] && continue;
      Server* serverconnection = NULL;
      serverconnection = g_connectionpool->get(connectionids_[i]);
      Assert(serverconnection);
      int32_t socketid = serverconnection->getsocket()->getid();
      socketid_ == socketid && continue;
      if (FD_ISSET(socketid, readfds_[kSelectUse])) {
        if (serverconnection->getsocket()->iserror()) {
          removeconnection(serverconnection);
        }
        else {
          try {
            !serverconnection->processinput() && 
              removeconnection(serverconnection);
          }
          catch(...) {
            removeconnection(serverconnection);
          }
        }
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ServerManager::processoutput() {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == maxfd_&& SOCKET_INVALID == minfd_)
      return false;
    uint16_t i;
    uint16_t connectioncount = connection::Manager::getcount();
    for (i = 0; i < connectioncount; ++i) {
      ID_INVALID == connectionids_[i] && continue;
      Server* serverconnection = NULL;
      serverconnection = g_connectionpool->get(connectionids_[i]);
      Assert(serverconnection);
      int32_t socketid = serverconnection->getsocket()->getid();
      socketid_ == socketid && continue;
      if (FD_ISSET(socketid, writefds_[kSelectUse])) {
        if (serverconnection->getsocket()->iserror()) {
          removeconnection(serverconnection);
        }
        else {
          try {
            !serverconnection->processcommand(false) && 
              removeconnection(serverconnection);
          }
          catch(...) {
            removeconnection(serverconnection);
          }
        }
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ServerManager::processexeption() {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == minfd_ && SOCKET_INVALID == maxfd_)
      return true;
    uint16_t connectioncount = connection::Manager::getcount();
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) {
      ID_INVALID == connectionids_[i] && continue;
      serverconnection = g_connectionpool->get(connectionids_[i]);
      Assert(serverconnection);
      int32_t socketid = serverconnection->getsocket()->getid();
      if (socketid_ == socketid) {
        Assert(false);
        continue;
      }
      if (FD_ISSET(socketid, &exceptfds_[kSelectUse])) {
        removeconnection(serverconnection);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ServerManager::processcommand() {
  __ENTER_FUNCTION
    if (SOCKET_INVALID == maxfd_&& SOCKET_INVALID == minfd_)
      return false;
    uint16_t i;
    uint16_t connectioncount = connection::Manager::getcount();
    for (i = 0; i < connectioncount; ++i) {
      ID_INVALID == connectionids_[i] && continue;
      Server* serverconnection = NULL;
      serverconnection = g_connectionpool->get(connectionids_[i]);
      Assert(serverconnection);
      int32_t socketid = serverconnection->getsocket()->getid();
      socketid_ == socketid && continue;
      if (FD_ISSET(socketid, writefds_[kSelectUse])) {
        if (serverconnection->getsocket()->iserror()) {
          removeconnection(serverconnection);
        }
        else {
          try {
            !serverconnection->processoutput() && 
              removeconnection(serverconnection);
          }
          catch(...) {
            removeconnection(serverconnection);
          }
        }
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ServerManager::accept_newconnection() {
  __ENTER_FUNCTION
    using namespace pap_server_common_gamne::define;
    uint8_t step = 0;
    bool result = false;
    Server* newconnection = NULL;
    newconnection = g_connectionpool->create();
    NULL == newconnection && return false;
    step = 5;
    newconnection->cleanup();
    int32_t socketid = SOCKET_INVALID;
    step = 10;
    try {
      //accept client socket
      result = serversocket_->accept(newconnection->getsocket());
      if (!result) {
        step = 15;
        goto EXCEPTION;
      }
    }
    catch(...) {
      step += 1000;
      goto EXCEPTION;
    }

    try {
      step = 30;
      socketid = newconnection->getsocket()->getid();
      if (SOCKET_INVALID == socketid) {
        Assert(false);
        goto EXCEPTION;
      }
      step = 40;
      result = newconnection->getsocket()->set_nonblocking();
      if (!result) {
        Assert(false);
        goto EXCEPTION;
      }
      step = 50;
      if (newconnection->getsocket()->iserror()) {
        Assert(false);
        goto EXCEPTION;
      }
      step = 60;
      result = newconnection->getsocket()->setlinger(0);
      if (!result) {
        Assert(false);
        goto EXCEPTION;
      }
      step = 70;
      newconnection->init();
      newconnection->setstatus(status::connection::kWorldConnect);
      step = 80;
      try {
        result = addconnection(newconnection);
        if (!result) {
          Assert(false);
          goto EXCEPTION;
        }
      }
      catch(...) {
        step += 10000;
        goto EXCEPTION;
      }
    }
    catch(...) {
      step += 100000;
    }
    g_log->fast_save_log(kBillingLogFile,
                         "ServerManager::accept_newconnection(socketid: %d)",
                         newconnection->getsocket()->getid());
    return true;
EXCEPTION:
    newconnection->cleanup();
    g_connectionpool->remove(newconnection->getid());
    return false;
  __LEAVE_FUNCTION
    return false;
}

bool ServerManager::heartbeat() {
  __ENTER_FUNCTION
    uint32_t currenttime = g_time_manager->get_current_time();
    uint16_t connectioncount = connection::Manager::getcount();
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) {
      ID_INVALID == connectionids_[i] && continue;
      pap_server_common_net::connection::Base* connection = NULL;
      connection = g_connectionpool->get(connectionids_[i]);
      if (NULL == connection) {
        Assert(false);
        return false;
      }
      if (!connection->heartbeat(currenttime)) {
        removeconnection(connection);
        Assert(false);
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

void ServerManager::loop() {
  __ENTER_FUNCTION
    while (isactive()) {
      bool result = false;
      uint32_t currenttime = g_time_manager->get_current_time();
      try {
        result = select();
        Assert(result);
        result = processexception();
        Assert(result);
        result = processinput();
        Assert(result);
        result = processoutput();
        Assert(result); 
      }
      catch(...) {
        
      }
      try {
        result = processcommand();
        Assert(result);
      }
      catch(...) {
        
      }

      try {
        result = heartbeat();
        Assert(result);
      }
      catch(...) {
      }
    }
  __LEAVE_FUNCTION
}

bool ServerManager::isactive() {
  return active_;
}

void ServerManager::setactive(bool active) {
  active_ = active;
}

bool ServerManager::addconnection(
    pap_server_common_net::connection::Base* connection) {
  __ENTER_FUNCTION
    if (fdsize_ > FD_SETSIZE) {
      Assert(false);
      return false;
    }
    if (!connection::Manager::add(connection)) {
      Assert(false);
      return false;
    }
    int32_t socketid = connection->getsocket()->getid();
    Assert(SOCKET_INVALID != socketid);
    minfd_ = min(socketid, minfd_);
    maxfd_ = max(socketid, maxfd_);
    FD_SET(socketid, &readfds_[kSelectFull]);
    FD_SET(socketid, &writefds_[kSelectFull]);
    FD_SET(socketid, &exceptfds_[kSelectFull]);
    ++fdsize_;
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ServerManager::deleteconnection(
    pap_server_common_net::connection::Base* connection) {
  __ENTER_FUNCTION
    Server* serverconnection;
    serverconnection = //class pointer transform use dynamic_cast will be safe
      dynamic_cast<Server*>(connection);
    int32_t socketid = serverconnection->getsocket()->getid();
    uint16_t i;
    Assert(minfd_ != SOCKET_INVALID && maxfd_ != SOCKET_INVALID);   
    if (socketid == minfd_) { //the first connection
      int32_t socketid_max = maxfd_;
      uint16_t connectioncount = connection::Manager::getcount();
      for (i = 0; i < connectioncount; ++i) {
        ID_INVALID == connectionids_[i] && continue;
        serverconnection = g_connectionpool->get(connectionids_[i]);
        Assert(serverconnection);
        NULL == serverconnection && continue;
        int32_t _socketid = serverconnection->getsocket()->getid();
        (socketid == _socketid || SOCKET_INVALID == _socketid) && continue;
        socketid_max < _socketid && socketid_max = _socketid;
        if (minfd_ == maxfd_) {
          minfd_ = maxfd_ = SOCKET_INVALID;
        }
        else {
          if (socketid_max > socketid_) {
            minfd_ = socketid_;
          }
          else {
            minfd_ = socketid_max;
          }
        }
      }
    }
    else if (socketid == maxfd_) { //
      int32_t socketid_min = minfd_;
      uint16_t connectioncount = connection::Manager::getcount();
      for (i = 0; i < connectioncount; ++i) {
        ID_INVALID == connectionids_[i] && continue;
        serverconnection = g_connectionpool->get(connectionids_[i]);
        Assert(serverconnection);
        NULL == serverconnection && continue;
        int32_t _socketid = serverconnection->getsocket()->getid();
        (socketid == _socketid || SOCKET_INVALID == _socketid) && continue;
        socketid_min > _socketid && socketid_min = _socketid;
        if (minfd_ == maxfd_) {
          minfd_ = maxfd_ = SOCKET_INVALID;
        }
        else {
          if (socketid_min < socketid_) {
            maxfd_ = socketid_;
          }
          else {
            maxfd_ = socketid_min;
          }
        }
      }
    }
    FD_CLR(socketid, &readfds_[kSelectFull]);
    FD_CLR(socketid, &readfds_[kSelectUse]);
    FD_CLR(socketid, &writefds_[kSelectFull]);
    FD_CLR(socketid, &writefds_[kSelectUse]);
    FD_CLR(socketid, &exceptfds_[kSelectFull]);
    FD_CLR(socketid, &exceptfds_[kSelectUse]);
    --fdsize_;
    Assert(fdsize_ >= 0);
    connection::Manager::remove(serverconnection->getid());
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool ServerManager::removeconnection(
    pap_server_common_net::connection::Base* connection) {
  __ENTER_FUNCTION
    //first clean in connection manager
    Assert(deleteconnection(connection));
    Server* serverconnection = NULL;
    serverconnection = 
      dynamic_cast<Server*>(connection);
    Assert(serverconnection != NULL);
    //second clean in connection pool, free to new connection
    serverconnection->freeown();
    g_log->fast_save_log(kBillingLogFile, 
                         "ServerManager::removeconnection(id: %d)", 
                         connection->getid());
    return true;
  __LEAVE_FUNCTION
    return false;
}

void ServerManager::remove_allconnection() {
  __ENTER_FUNCTION
    uint16_t connectioncount = connection::Manager::getcount();
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) {
      ID_INVALID == connectionids_[0] && break;
      pap_server_common_net::connection::Base* connection = NULL;
      connection = g_connectionpool->get(connectionids_[i]);
      if (NULL == connection) {
        Assert(false);
        break;
      }
      removeconnection(connection);
    }
  __LEAVE_FUNCTION
}

Server* 
  ServerManager::get_serverconnection(uint16_t id) {
  __ENTER_FUNCTION 
    Assert(id >= 0 && id < OVER_SERVER_MAX);
    int32_t connectionid = serverhash_[id];
    Server* serverconnection = NULL;
    serverconnection = g_connectionpool->get(connectionid);
    Assert(serverconnection);
    return serverconnection;
  __ENTER_FUNCTION
    return NULL;
}

void ServerManager::broadcast(pap_server_common_net::packet::Base* packet) {
  __ENTER_FUNCTION
    uint16_t connectioncount = connection::Manager::getcount();
    uint16_t i;
    for (i = 0; i < connectioncount; ++i) {
      ID_INVALID == connectionids_[i] && continue;
      pap_server_common_net::connection::Base* connection = NULL;
      connection = g_connectionpool->get(connectionids_[i]);
      if (NULL == connection) {
        Assert(false) && continue;
      }
      connection->sendpacket(packet);
    }
  __LEAVE_FUNCTION
}
