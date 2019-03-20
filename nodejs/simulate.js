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

function CreateEngine () {
    let engine = new vox.VoxEngine()
    return engine
}

const data = '/Users/qwu/Work/projects/vidi/dxserver/data/vorts1.json'
const rqstFrame = 100
let   receFrame = 0
let Ts, Te;

function MainLoop (key) {

    let engine = CreateEngine()

    engine.OnConnect = () => {
        console.log('connected')
        engine.NotifyClientKey(key) // Start the process by sending the old client key
        engine.CallQueryDatabase()
        engine.NotifyOpenProject(data)
    }

    engine.OnProjectOpened = () => {
        console.log('opened')
        engine.CallGetScene()
        Ts = new Date().getTime()
        for (let i = 0; i < rqstFrame; ++i) {
            engine.NotifyRequestFrame(null)
        }
    }
    engine.OnFrame = (frame) => {
        console.log('hasNewFrame')
        ++receFrame;
        if (receFrame === rqstFrame) {
            Te = new Date().getTime()
            const es = (Te - Ts) / 1000
            console.log(`average frame rate over ${rqstFrame} caused ${es}s frames = ${rqstFrame / es}`);
        }
        // console.log('hasNewFrame', frame)
    }
    engine.OnGetScene = (scene) => {
        console.log('hasNewScene', scene)
    }
    engine.OnQueryDatabase = (database) => {
        console.log('queryDatabase')
        // console.log('queryDatabase', database)
    }
    engine.OnClientKey = (key) => {
        console.log('setClientKey', key['params']['key'])
    }

    // seriously, let's connect
    engine.connect(remote)

}

MainLoop('yheo12u398123yo13jr912gho')