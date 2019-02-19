/* eslint-disable no-console */
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
class VoxClient {
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
    // Create an array to store clients
    clients = {}
    this._io.on('connection', (socket) => {
      clients[socket.id] = socket
      console.log('client connection' + socket.id)
      // console.log('client connection')
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
module.exports = VoxClient
