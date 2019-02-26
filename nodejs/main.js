/* eslint-disable no-console */
const vox = require('./module/index')

let PullFromEngine = () => {
  vox.engine.CallGetScene()
  vox.engine.NotifyRequestFrame(null)
}

function CreateEngine (client_id) {
  let engine = new vox.VoxEngine()
  engine.OnConnect = () => {
    console.log('connected', client_id)
  }
  engine.OnProjectOpened = () => {
    console.log('opened', client_id)
  }
  engine.connect('localhost')
  return engine
}
// let clients = new Map()
// var id = 1
// for(let i = 0; i < 2; ++i) {
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
    //vox.client.Broadcast('hasNewFrame', frame)
  }
  engine.OnGetScene = (scene) => {
    console.log('has new scene')
    socket.emit('hasNewScene', scene)
    // clientsAction(vox.client.clients.get(socket), )
  }
  engine.OnQueryDatabase = (database) => {
    socket.emit('queryDatabase', database)
  }
  //
  // setup socket
  //
  socket.on('selectData', (data) => {
    //vox.engine.NotifyOpenProject(data.label)
    //PullFromEngine()
  })
  socket.on('disconnect', () => {

    // delete clients[socket.id]
    // vox.engine.NotifyCloseProject()
    // PullFromEngine()
    console.log('disconnect', client_id)
    if(vox.clients.has(client_id)) {
      vox.clients.delete(client_id)
    }
    if(vox.engines.has(client_id)) {
      vox.engines[client_id].close()
      vox.engines.delete(client_id)
    }

  })
  socket.on('requestFrame', (params) => {
    //vox.engine.NotifyRequestFrame(params)
    // PullFromEngine()
  })
  //
  // Start the process
  //
  //vox.engine.CallQueryDatabase()
  //PullFromEngine()
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

