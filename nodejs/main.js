//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

const remote = process.argv.length > 2 ? process.argv[2] : 'localhost'

/* eslint-disable no-console */
const vox = require('./module/index')

function CreateEngine (client_id) {
  let engine = new vox.VoxEngine()
  return engine
}

function MainLoop(socket) {

  socket.on('clientKey', (key) => {

    let client_id;
    let engine;
    vox.lock.acquire(0, function () {
      client_id = vox.next
      vox.next += 1
      engine = CreateEngine(client_id)
      vox.clients[client_id] = socket
      vox.engines[client_id] = engine
    })

    engine.OnConnect = () => {
      console.log('connected', client_id)
      engine.NotifyClientKey(key) // Start the process by sending the old client key
      engine.CallQueryDatabase()
      engine.CallGetScene()
      engine.NotifyRequestFrame(null)
    }
    engine.OnProjectOpened = () => {
      console.log('opened', client_id)
      engine.CallGetScene()
      engine.NotifyRequestFrame(null)
    }
    engine.OnFrame = (frame) => {
      socket.emit('hasNewFrame', frame)
    }
    engine.OnGetScene = (scene) => {
      console.log('has new scene')
      socket.emit('hasNewScene', scene)
    }
    engine.OnQueryDatabase = (database) => {
      socket.emit('queryDatabase', database)
    }
    engine.OnClientKey = (key) => {
      console.log('setClientKey', key['params']['key'])
      socket.emit('setClientKey', key['params']['key'])
    }

    socket.on('selectData', (data) => {
      engine.NotifyOpenProject(data.label)
      engine.CallGetScene()
      engine.NotifyRequestFrame(null)
    })
    socket.on('disconnect', () => {
      console.log('disconnect', client_id)
      engine.close()
      if (vox.clients.has(socket)) {
        vox.clients.delete(socket)
      }
      if (vox.engines.has(client_id)) {
        vox.engines.delete(client_id)
      }
    })
    socket.on('requestFrame', (params) => {
      engine.NotifyRequestFrame(params)
    })
    socket.on('exchangeKey', (key) => {
      engine.NotifyExchangeKey(key)
    })
    socket.on('clientKey', (key) => {
      console.log('client key', key)
    })

    // seriously, let's connect
    engine.connect(remote)

  })

  socket.emit('getClientKey')

}

vox.webserver.OnConnect = MainLoop
vox.webserver.serve(4000)
