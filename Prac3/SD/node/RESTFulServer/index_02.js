'use strict'

const port = process.env.PORT || 8888;
const express = require('express');
const app = express();

app.get('/hola/:unNombre', (req, res) => {
	res.status(200).send({ mensaje: `¡Hola ${req.params.unNombre} desde SD!` });
});

app.listen(port, () => {
	console.log(`API REST ejecutándose en http://localhost:${port}/hola/:unNombre`);
});