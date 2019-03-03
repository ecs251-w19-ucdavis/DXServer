/* eslint-disable no-console */

//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//
/*!
 * VoxEngine
 * Copyright(c) 2018 Qi Wu (Wilson)
 * MIT Licensed
 */

'use strict'

/**
 * Module dependencies.
 */
const WebSocket = require('websocket').w3cwebsocket
const JsonRPC = require('./JsonRPC')

/**
 * Connection between server and rendering engine.
 *
 * @return {class}
 * @api public
 */
class VoxEngine {
  /**
   * @constructor
   */
  constructor () {
    // fields
    this._engine = undefined
    // functions
    this._OnOpen = undefined
    // signal handlers [naming: _Handler_<signal>_]
    this._Handler_frame_ = undefined
    this._Handler_queryDatabase_ = undefined
    this._Handler_getScene_ = undefined
    this._Handler_projectOpened_ = (msg) => {
      console.log('project opened')
    }
    this._Handler_projectClosed_ = (msg) => {
      console.log('project closed')
    }
  }

  /**
   * Helper function to send a Json-RPC notification to the engine
   * @param method
   * @param params
   */
  _rpcNotify (method, params) {
    const msg = JsonRPC.Notify(method, params)
    this._engine.send(JSON.stringify(msg))
  }

  /**
   * Helper function to make a Json-RPC call to the engine
   * @param method
   * @param params
   * @param id
   * @param response
   */
  _rpcCall (method, params, id, response) {
    const msg = JsonRPC.Call(method, params, response, id)
    this._engine.send(JSON.stringify(msg))
  }

  /**
   * Handlers
   */
  /** Call: queryDatabase */
  CallQueryDatabase () {
    /* TODO hacking the index now */
    this._rpcCall('queryDatabase', {}, 1, this._Handler_queryDatabase_)
  }

  set OnQueryDatabase (func) {
    this._Handler_queryDatabase_ = func
  }

  /** Call: getScene */
  CallGetScene () {
    /* TODO hacking the index now */
    this._rpcCall('getScene', {}, 2, this._Handler_getScene_)
  }

  set OnGetScene (func) {
    this._Handler_getScene_ = func
  }

  /** Notify: openProject */
  NotifyRequestFrame (params) {
    this._rpcNotify('requestFrame', params)
  }

  set OnFrame (func) {
    this._Handler_frame_ = func
  }

  /** Notify: openProject */
  NotifyOpenProject (project) {
    this._rpcNotify('openProject', { fileName: project })
  }

  set OnProjectOpened (func) {
    this._Handler_projectOpened_ = func
  }

  /** Notify: closeProject */
  NotifyCloseProject () {
    this._rpcNotify('closeProject', null)
  }

  set OnProjectClosed (func) {
    this._Handler_projectClosed_ = func
  }

  /**
   *
   * @param event
   */
  _OnMessage (event) {
    // parse event into as a json string
    let msg = JSON.parse(event.data)
    // check if it is a call message
    if (JsonRPC.CallCheck(msg)) return
    // check if it is a rendered image
    if ('method' in msg) {
      switch (msg.method) {
        case 'frame': {
          if (typeof (this._Handler_frame_) !== 'undefined') {
            this._Handler_frame_(msg.params.data)
          }
          break
        }
        case 'projectOpened': {
          this._Handler_projectOpened_(msg)
          break
        }
        case 'projectClosed': {
          this._Handler_projectClosed_(msg)
          break
        }
        default: {
          console.log('unknown method', msg)
        }
      }
    } else {
      console.log('unknown messages', msg)
    }
  }

  /**
   * Function to execute when connecting/disconnecting to the render engine
   * @param func
   */
  set OnConnect (func) {
    this._OnOpen = func
  }

  /**
   * How should we connect to the engine
   * @param hostname
   */
  connect (hostname) {
    // create the connect between server and the rendering engine using port 8080
    // TODO check connection error
    this._engine = new WebSocket('ws://' + hostname + ':8080')

    // TODO: warn if port 8080 has been occupied
    console.log('Render engine running on ' + hostname + ':8080')

    // register open actions for the engine
    this._engine.onopen = () => {
      this._OnOpen()
    }

    // register message actions for the engine
    this._engine.onmessage = (msg) => {
      this._OnMessage(msg)
    }

    // register close actions for the engine
    this._engine.onclose = () => {
      console.log('Render engine connection has been closed')
    }

    // register error actions for the engine
    this._engine.onerror = () => {
      console.log('Connection Error')
    }
  }

  close () {
    this._engine.onclose = () => {}
    this._engine.close()
  }
}

/**
 * Expose `VoxEngine class`.
 */
module.exports = VoxEngine
