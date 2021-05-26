const Net = require('net');

const port = 40110;
const host = '192.168.122.1';

const client = new Net.Socket();

client.connect({ port: port, host: host }, () => {
    console.log('TCP connection established with the server.')
})

client.on('data', chunk => {
	data = JSON.parse(chunk)

	// send heartbeat
	if (data.op === 0) 
		setInterval(() => client.write('{"op":0}'), data.interval)


    console.log(data)
})

client.on('end', () => {
	// if the connection closed, try to connect again
	setInterval(() => client.connect({ port: port, host: host }), 1000)
})