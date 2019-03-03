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
const Express = require('express')
const Path = require('path')
const Http = require('http')
const SocketIO = require('socket.io')

/**
 * Connection between web interface and server
 *
 * @return {class}
 * @api public
 */
class VoxWebserver {
  // clients = {1 : null, 2 : null}
  constructor () {
    this._app = Express()
    this._http = Http.Server(this._app)
    this._io = SocketIO(this._http)
    this._app.use(Express.static(Path.resolve(__dirname, '../../external/webvidi3d/dist')))
  }

  serve (port) {
    this._http.listen(port, function () {
      console.log('listening on http://localhost:' + port)
    })
  }

  set OnConnect (func) {
    this._io.on('connection', (socket) => {
      console.log('client connection', socket.id)
      func(socket)
    })
  }

  Broadcast (signal, message) {
    this._io.emit(signal, message)
  }
}

/**
 * Expose `VoxEngine class`.
 */
module.exports = VoxWebserver
