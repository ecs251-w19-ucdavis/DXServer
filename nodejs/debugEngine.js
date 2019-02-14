/* eslint-disable no-console */
const vox = require('./module/index')
vox.engine.OnConnect = () => {
    console.log('connect')
    vox.engine._rpcCall('getScene', {}, 2, (data) => {
	console.log(data)
    })
}
vox.engine.connect(process.argv.length > 2 ? process.argv[2] : 'localhost')
