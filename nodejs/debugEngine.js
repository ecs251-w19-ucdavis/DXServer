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
    en0._rpcNotify('openProject', "engine.raw");
    en0._rpcNotify('requestFrame', {})
}
en0.connect('localhost')

