/* eslint-disable no-console */
/*!
 * VoxEngine
 * Copyright(c) 2018 Qi Wu (Wilson)
 * MIT Licensed
 */
// JSON RPC 2.0 Specification
// link: https://www.jsonrpc.org/specification

'use strict'

/*
 * Maintaining a local variable that stores all call id and call response
 */
let rpcCallList = [/* id, response */]

module.exports = {
  CallCheck: (msg) => {
    if ('result' in msg && 'id' in msg) {
      for (let i = 0; i < rpcCallList.length; ++i) {
        if (msg.id === rpcCallList[i].id) {
          rpcCallList[i].response(msg.result)
          rpcCallList.splice(i, 1)
          return true
        }
      }
    }
    return false
  },
  Call: (method, params, response, id) => {
    rpcCallList.push({
      id: id,
      response: response
    })
    return {
      jsonrpc: '2.0',
      method: method,
      params: params,
      id: id
    }
  },
  Notify: (method, params) => {
    return {
      jsonrpc: '2.0',
      method: method,
      params: params
      // no id because it's a notification
    }
  }
}
