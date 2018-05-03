var http = require('http');

http.createServer( (request, response) => {
	response.writeHead(200, {'Content-Type': 'text/plain'});
	response.end('Hola a todas y a todos\n');
}).listen(8080)
console.log('Servidor ejecutándose en puerto 8080');