//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//

/* eslint-disable no-console */
const vox = require('./module/index')

function CreateEngine (client_id) {
  let engine = new vox.VoxEngine()
  engine.OnConnect = () => {
    console.log('connected', client_id)
    // Start the process
    engine.CallQueryDatabase()
    engine.CallGetScene()
    engine.NotifyRequestFrame(null)
  }
  engine.OnProjectOpened = () => {
    console.log('opened', client_id)
    engine.CallGetScene()
    engine.NotifyRequestFrame(null)
  }
  engine.connect('localhost')
  return engine
}

function MainLoop(socket) {

  let client_id;
  let engine;
  vox.lock.acquire(0, function () {
    client_id = vox.next
    vox.next += 1
    engine = CreateEngine(client_id)
    vox.clients[client_id] = socket
    vox.engines[client_id] = engine
  })

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

  socket.on('selectData', (data) => {
    engine.NotifyOpenProject(data.label)
    engine.CallGetScene()
    engine.NotifyRequestFrame(null)
  })
  socket.on('disconnect', () => {
    console.log('disconnect', client_id)
    engine.close()
    if(vox.clients.has(socket)) {
     vox.clients.delete(socket)
    }
    if(vox.engines.has(client_id)) {
     vox.engines.delete(client_id)
    }
  })
  socket.on('requestFrame', (params) => {
    engine.NotifyRequestFrame(params)
  })

}

vox.webserver.OnConnect = MainLoop
vox.webserver.serve(4000)
