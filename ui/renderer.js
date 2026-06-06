// === CONFIGURACION DE DATOS DEL LOG (TESTING) ===
let dnaSequence = "";
let logData = [];

let currentStep = 0;
let isPlaying = false;

/*  ====== TODO: ======
 * 
 * Agregar función para cargar el .log y "parsear" su contenido
 * - El .log debe contener información de cada hilo, su patrón, posición y resultado (match/miss)
 * - El formato del .log debe ser consistente para facilitar su lectura (ej: JSON, CSV, etc.)
 * - La función de carga debe actualizar las variables para que el renderer pueda utilizarlas
 * 
 * Ejemplo de formato de .log (JSON):
{
    "dna": "ATGCGTATGAAACCCTAGTACTG",
    "patterns": [
            "ATG",
            "CGT",
            "AAA",
            "CCC"
            ...
    ],
    "mode": "PTHREAD",
    "n_ids": 4, // Para identificar el número de hilos/procesos involucrados
    "logs": [
        {"id": 1, "pattern": 0, "pos": 0, "match": 1},
        {"id": 2, "pattern": 1, "pos": 0, "match": 0},
        ...
    ]
}
 *
 * Crear cache  tipo "state" (similar a game of life) para almacenar el estado de cada hilo/proceso
 * - - Esto permitirá mostrar el progreso de cada hilo a lo largo del tiempo, permitiendo visualizar cómo avanzan en la secuencia de ADN
 * - El estado de cada hilo/proceso se puede representar con colores para facilitar su seguimiento
 * 
 * Agregar listado de patrones con sus respectivas posiciones y resultados (match/miss) para cada hilo/proceso
 * - - Esto permitirá mostrar un resumen de la actividad de cada hilo/proceso, facilitando la comprensión de su comportamiento a lo largo del tiempo
 * - El listado puede mostrarse en una sección lateral o inferior del canvas para complementar la visualización principal
 * - El listado debe actualizarse dinámicamente a medida que se avanza en la línea de tiempo, reflejando el estado actual de cada hilo/proceso
 * - El listado 
 * 
 * Ajustar impresión y márgenes del canvas de manera automática para adaptarse a diferentes tamaños de pantalla y resoluciones
 * - - Esto garantizará que la visualización sea clara y legible en diferentes resoluciones.
 * - Se pueden utilizar técnicas de diseño responsivo para adaptar el tamaño de los elementos visuales (hilos, patrones, texto) en función del tamaño del canvas.
 * - Es importante mantener un equilibrio entre la cantidad de información mostrada y la claridad visual.
 * - Se puede implementar una función de "zoom" o "scroll" para permitir la exploración detallada de la secuencia de ADN y los patrones.
 * - Dinamizar impresión de la secuencia de ADN para mostrar solo una parte visible a la vez mediante segmentos desplazables verticalmente u horizontalmente.
 * 
 * AJUSTE EN EL PROGRAMA:
 * - Agregar las funciones de generación de el/los .logs para probar la visualización con datos reales.
 * 
*/

// Paleta de colores para hilos y estados
const colors = {
    bg: '#1e1e2e',
    text: '#cdd6f4',
    match: '#a6e3a1',
    miss: '#f38ba8',
    hilo1: '#89b4fa',
    thrd2: '#f9e2af',
    thrd3: '#cba6f7',
    thrd4: '#fab387'
};

function setup() {
    createCanvas(1000, 400);
    frameRate(10); // 10 cuadros por segundo para reproducoción suave
    textAlign(CENTER, CENTER);
    textSize(16);
    
    // Dataset de prueba - Agregar lectura del .log AQUÍ -
    dnaSequence = "ATGCGTATGAAACCCTAGTACTG";
    logData = [
        { mode: "PTHREAD", id: 1, pat: "ATG", pos: 0, match: 1 },
        { mode: "PTHREAD", id: 2, pat: "CGT", pos: 0, match: 0 },
        { mode: "PTHREAD", id: 1, pat: "ATG", pos: 1, match: 0 },
        { mode: "PTHREAD", id: 2, pat: "CGT", pos: 1, match: 0 },
        { mode: "PTHREAD", id: 2, pat: "CGT", pos: 3, match: 1 }, 
    ];
}

function draw() {
    background(colors.bg);
    
    drawDNA();

    // Control de flujo de la línea de tiempo
    if (isPlaying) {
        if (currentStep < logData.length) {
            currentStep++;
        } else {
            isPlaying = false;
            document.getElementById("playBtn").innerText = "▶";
        }
    }

    // 3. Renderizar el cuadro activo del .log
    let stepIndex = isPlaying ? currentStep - 1 : currentStep;
    if (stepIndex >= 0 && stepIndex < logData.length) {
        drawScanner(logData[stepIndex]);
    }
}

function drawDNA() {
    let boxWidth = 40;
    for (let i = 0; i < dnaSequence.length; i++) {
        fill(colors.bg);
        stroke(colors.text);
        rect(50 + i * boxWidth, 150, boxWidth, 40, 4);
        
        noStroke();
        fill(colors.text);
        text(dnaSequence[i], 50 + i * boxWidth + boxWidth/2, 170);
    }
}

function drawScanner(step) {
    let boxWidth = 40;
    let startX = 50 + step.pos * boxWidth;
    let patWidth = step.pat.length * boxWidth;
    
    let thrdColor = colors['hilo' + ((step.id % 4) + 1)];
    
    fill(step.match ? colors.match : thrdColor + '44'); 
    stroke(step.match ? colors.match : thrdColor);
    strokeWeight(3);
    
    rect(startX, 150, patWidth, 40, 4);
    
    noStroke();
    fill(colors.text);
    text(`Hilo ${step.id} analizando: ${step.pat}`, startX + patWidth/2, 130);
}

// =========== CONTROL DE EVENTOS (reproducción) ===========

function togglePlay() {
    isPlaying = !isPlaying;
    let btn = document.getElementById("playBtn");
    btn.innerText = isPlaying ? "⏸" : "▶";
}

function stepForward() {
    if (isPlaying) togglePlay();
    if (currentStep < logData.length - 1) currentStep++;
}

function stepBackward() {
    if (isPlaying) togglePlay();
    if (currentStep > 0) currentStep--;
}

function resetSim() {
    currentStep = 0;
    isPlaying = false;
    document.getElementById("playBtn").innerText = "▶";
}