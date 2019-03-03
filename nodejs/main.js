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

/* TODO what if there are multiple sockets connecting */

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
  //
  // setup socket
  //
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

/*
 * Client Section
 */

/* We have to run the server first */
// if(vox.client.clients.size === 0) {
//   vox.client.OnConnect = MainLoop
//   vox.engine.OnConnect = () => {
//     vox.client.serve(4000)
// }
//   vox.engine.OnProjectOpened = () => {
//     console.log('opened')
//     PullFromEngine()
// }
//   vox.engine.connect(process.argv.length > 2 ? process.argv[2] : 'localhost')
// } else {
//   while(vox.client.clients.size !== 0) {
//     vox.client.OnConnect = MainLoop
//     vox.engine.OnConnect = () => {
//       vox.client.serve(4000)
//     }
//     vox.engine.OnProjectOpened = () => {
//       console.log('opened')
//       PullFromEngine()
//     }
//     vox.engine.connect(process.argv.length > 2 ? process.argv[2] : 'localhost')
//   }
// }


vox.webserver.OnConnect = MainLoop
vox.webserver.serve(4000)

//vox.engine.OnConnect = () => {
//
//}
//vox.engine.OnProjectOpened = () => {
//  console.log('opened')
//  PullFromEngine()
//}
//vox.engine.connect(process.argv.length > 2 ? process.argv[2] : 'localhost')

//if(clients[socket] == null) {
//  clients[socket] = 1
//  engines[socket] = 1
//} else {
//  clients[socket] = parseInt(clients[socket]) + 1
//  engines[socket] = parseInt(engines[socket]) + 1
//}

