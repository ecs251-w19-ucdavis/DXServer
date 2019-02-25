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

  clients = new Map()

  set OnConnect (func) {
    this._io.on('connection', (socket) => {
      // clients[socket.id] = socket
      if(clients[socket] == null) {
        clients[socket] = 1
      } else {
        clients[socket] = parseInt(clients[socket]) + 1
      }
      console.log('client connection')
      func(socket)
      // func(socket)
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
