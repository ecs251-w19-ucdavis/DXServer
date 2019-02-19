/* eslint-disable no-console */
const vox = require('./module/index')

let PullFromEngine = () => {
  vox.engine1.CallGetScene()
  vox.engine1.NotifyRequestFrame(null)
}

for(let i = 0; i < 2; ++i) {
  /* TODO what if there are multiple sockets connecting */
const MainLoop = (socket) => {
  
  //
  // setup engine
  //
  
  vox.engine[i].OnFrame = (frame) => {
    vox.client[i].Broadcast('hasNewFrame', frame)
  }
  vox.engine[i].OnGetScene = (scene) => {
    console.log('has new scene')
    socket.emit('hasNewScene', scene)
  }
  vox.engine[i].OnQueryDatabase = (database) => {
    socket.emit('queryDatabase', database)
  }
  //
  // setup socket
  //
  socket.on('selectData', (data) => {
    vox.engine[i].NotifyOpenProject(data.label)
    PullFromEngine()
  })
  socket.on('disconnect', () => {
    // delete clients[socket.id]
    // vox.engine.NotifyCloseProject()
    // PullFromEngine()
  })
  socket.on('requestFrame', (params) => {
    vox.engine[i].NotifyRequestFrame(params)
    // PullFromEngine()
  })
  //
  // Start the process
  //
  vox.engine[i].CallQueryDatabase()
  PullFromEngine()
}

/*
 * Client Section
 */

/* We have to run the server first */
vox.client[i].OnConnect = MainLoop
vox.engine[i].OnConnect = () => {
  vox.client[i].serve(4000)
}
vox.engine[i].OnProjectOpened = () => {
  console.log('opened')
  PullFromEngine()
}
vox.engine[i].connect(process.argv.length > 2 ? process.argv[2] : 'localhost')
}

