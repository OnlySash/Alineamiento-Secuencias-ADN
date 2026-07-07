const express = require('express');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 3000;

app.use(express.static(path.join(__dirname)));

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'visualizer.html'));
});

app.get('/run-simulation', (req, res) => {
    const { n, k, l, t, m } = req.query;

    let command = `./bin/dna_search -n ${n} -k ${k} -l ${l} -m ${m}`;
    if (m === '2') command += ` -t ${t}`;
    if (m === '3') command = `mpirun -np ${t} ${command}`;

    console.log(`Ejecutando: ${command}`);

    exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error de ejecución: ${error.message}`);
            return res.status(500).json({ error: 'Fallo al ejecutar el código C.' });
        }

        const jsonPath = path.join(__dirname, 'data.json');
        fs.readFile(jsonPath, 'utf8', (err, data) => {
            if (err) return res.status(500).json({ error: 'No se encontró el archivo JSON.' });
            res.json(JSON.parse(data));
        });
    });
});

app.listen(PORT, () => {
    console.log(`Servidor UI activo en http://localhost:${PORT}`);
});