'use strict'

const express = require('express');
const app = express();

app.get('/hola', (req, res) => {
	res.send('¡Hola a Todas y a Todos desde Express!');
});

app.listen(8080, () => {
	console.log('API REST ejecutándose en http://localhost:8080');
});