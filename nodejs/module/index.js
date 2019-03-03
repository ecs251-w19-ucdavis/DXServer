/* eslint-disable no-console */

//===========================================================================//
//                                                                           //
// Daxian Server                                                             //
// Copyright(c) 2018 Qi Wu, Yiran Li, Wenxi Lu                               //
// University of California, Davis                                           //
// MIT Licensed                                                              //
//                                                                           //
//===========================================================================//
/*!
 * VoxEngine
 * Copyright(c) 2018 Qi Wu (Wilson)
 * MIT Licensed
 */

'use strict'

const VoxWebserver = require('./VoxClient')
const AsyncLock = require('async-lock')
module.exports = {
  VoxEngine: require('./VoxEngine'),
  // fields
  webserver: new VoxWebserver(),
  clients: new Map(),
  engines: new Map(),
  next: 1,
  lock: new AsyncLock()
};
