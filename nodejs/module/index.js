/* eslint-disable no-console */
/*!
 * VoxEngine
 * Copyright(c) 2018 Qi Wu (Wilson)
 * MIT Licensed
 */

'use strict'

const VoxClient = require('./VoxClient')
const VoxEngine = require('./VoxEngine')

module.exports = {
  client: new VoxClient(),
  engine: new VoxEngine()
};
