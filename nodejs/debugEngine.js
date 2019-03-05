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

let en0 = new vox.VoxEngine()
en0.OnConnect = () => {
    console.log('connect')
    en0._rpcNotify('openProject', {});
    en0._rpcCall('getScene', {}, 2, (data) => {
        console.log('engine 0', data)
    })
    en0._rpcNotify('requestFrame', {})
}
en0.connect('localhost')

let en1 = new vox.VoxEngine()
en1.OnConnect = () => {
    console.log('connect')
    en1._rpcCall('getScene', {}, 2, (data) => {
        console.log('engine 1', data)
    })
}
en1.connect('localhost')
