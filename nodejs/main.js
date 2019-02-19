/* eslint-disable no-console */
const vox = require('./module/index')

let PullFromEngine = () => {
  vox.engine.CallGetScene()
  vox.engine.NotifyRequestFrame(null)
}

/* TODO what if there are multiple sockets connecting */
const MainLoop = (socket) => {
  //
  // setup engine
  //
  vox.engine.OnFrame = (frame) => {
    vox.client.Broadcast('hasNewFrame', frame)
  }
  vox.engine.OnGetScene = (scene) => {
    console.log('has new scene')
    socket.emit('hasNewScene', scene)
  }
  vox.engine.OnQueryDatabase = (database) => {
    socket.emit('queryDatabase', database)
  }
  //
  // setup socket
  //
  socket.on('selectData', (data) => {
    vox.engine.NotifyOpenProject(data.label)
    PullFromEngine()
  })
  socket.on('disconnect', () => {
    // delete clients[socket.id]
    // vox.engine.NotifyCloseProject()
    // PullFromEngine()
  })
  socket.on('requestFrame', (params) => {
    vox.engine.NotifyRequestFrame(params)
    // PullFromEngine()
  })
  //
  // Start the process
  //
  vox.engine.CallQueryDatabase()
  PullFromEngine()
}

/*
 * Client Section
 */

/* We have to run the server first */
vox.client.OnConnect = MainLoop
vox.engine.OnConnect = () => {
  vox.client.serve(4000)
}
vox.engine.OnProjectOpened = () => {
  console.log('opened')
  PullFromEngine()
}
vox.engine.connect(process.argv.length > 2 ? process.argv[2] : 'localhost')
