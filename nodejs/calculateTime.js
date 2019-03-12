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
var start = new Date().getTime();
en0.OnConnect = () => {  
    console.log('connect')
    en0._rpcNotify('openProject', {});
    en0._rpcCall('getScene', {}, 2, (data) => {
        console.log('engine 0', data)
    })
    en0._rpcNotify('requestFrame', {
        "camera" : {
            "center" : {
            //   "x" : 127.5,
            //   "y" : 127.5,
            //   "z" : 63.5
              "x" : 300,
              "y" : 127.5,
              "z" : 63.5
            },
            "eye" : {
              "x" : 23.094518798354173,
              "y" : 34.884103052276984,
              "z" : -185.84885928678702
            },
            "fovy" : 45,
            "projectionMode" : "PERSPECTIVE",
            "up" : {
              "x" : -0.92766575691311226,
              "y" : 0.049177545754994084,
              "z" : 0.37015917176849472
            },
            "zFar" : 2550,
            "zNear" : 2.5500000000000003
          }
    })
}
en0.connect('localhost')
var elapsed = new Date().getTime() - start;
console.log(elapsed);

let en1 = new vox.VoxEngine()
var start = new Date().getTime();
en1.OnConnect = () => {
    console.log('connect')
    en1._rpcCall('getScene', {}, 2, (data) => {
        console.log('engine 1', data)
    })
}
en1.connect('localhost')
var elapsed = new Date().getTime() - start;
console.log(elapsed);
