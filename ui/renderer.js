/*  ====== TODO: ======
 * - Ajustar representación en la cantidad de hilos y su color asociado para que esta sea dinámica
 ** y no esté limitada a 4 hilos/procesos.
 * - Agregar el modo MPI (secuencial y pthreads comparten esta implementación)
 *
 * AJUSTE EN EL PROGRAMA:
 * - Agregar las funciones de generación de el/los .logs para probar la visualización con datos reales.
*/
// === CFG GRAFICA GLOBAL ===
const CONFIG = {
    // Dimensiones del Lienzo
    canvasWidth:    1000,
    canvasHeight:   700,
    
    // Cadena de ADN
    dnaStartX:          50,
    dnaStartY:          50,
    dnaBoxSize:         40,
    dnaSegmentLength:   20, // Nucleótidos máximos por fila
    dnaRowSpacing:      80, // Espacio entre cada fila de ADN
    
    // Impresión de patrones
    queueStartY:        450, // Altura donde empiezan a dibujarse los patrones
    queueBoxSize:       30,  // Tamaño de las cajas de los patrones
    queueStartX:        50,  // Margen izquierdo
    queueLimitX:        950, // Margen derecho (para salto de línea)
    queueRowSpacing:    50,  // Espaciado entre filas de patrones
    queueSpacing:       25   // Espaciado entre un patrón y otro
};

// Paleta de Colores
const COLORS = {
    bg:     '#1e1e2e',
    text:   '#cdd6f4',
    match:  '#a6e3a1',
    miss:   '#f38ba8',
    thrd1:  '#89b4fa',
    thrd2:  '#f9e2af',
    thrd3:  '#cba6f7',
    thrd4:  '#fab387'
};

// === ESTADO GLOBAL DE DATOS ===
let dnaSequence = "";
let patterns    = [];
let logData     = [];
let mode        = "";
let numIds      = 0;

let currentStep = 0;
let isPlaying   = false;
let playBtn;

function setup() {
    let canvas = createCanvas(CONFIG.canvasWidth, CONFIG.canvasHeight);
    let container = document.getElementById('canvas-container');
    if (container) canvas.parent('canvas-container');
    
    frameRate(10);
    textAlign(CENTER, CENTER);
    textSize(16);
    
    playBtn = document.getElementById("playBtn");
    dnaSequence = "";
}

function draw() {
    background(COLORS.bg);
    
    if (dnaSequence.length > 0) drawDNA();

    if (isPlaying && logData.length > 0) {
        if (currentStep < logData.length) currentStep++;
        else {
            if(playBtn) playBtn.innerText = "▶";
            isPlaying = false;
        }
    }

    let stepIndex = isPlaying ? currentStep - 1 : currentStep;
    
    if (logData.length > 0 && stepIndex >= 0 && stepIndex < logData.length) {
        drawScanner(logData[stepIndex]);
    }

    if (patterns.length > 0) drawPatternQueue(stepIndex);
}

function drawDNA() {
    for (let i = 0; i < dnaSequence.length; i++) {
        let col = i % CONFIG.dnaSegmentLength;
        let row = Math.floor(i / CONFIG.dnaSegmentLength);
        
        let x = CONFIG.dnaStartX + col * CONFIG.dnaBoxSize;
        let y = CONFIG.dnaStartY + row * CONFIG.dnaRowSpacing;

        fill(COLORS.bg);
        stroke(COLORS.text);
        strokeWeight(1);
        rect(x, y, CONFIG.dnaBoxSize, CONFIG.dnaBoxSize, 4);
        
        noStroke();
        fill(COLORS.text);
        text(dnaSequence[i], x + CONFIG.dnaBoxSize/2, y + CONFIG.dnaBoxSize/2);
    }
}

function drawScanner(step) {
    let thrdColor = COLORS['thrd' + ((step.id % 4) + 1)];
    let isMatch = (step.match === 1);
    
    let patternObj = patterns[step.patId];
    if (!patternObj || !patternObj.seq) return;
    let targetString = patternObj.seq;
    
    fill(isMatch ? COLORS.match + 'AA': thrdColor + '44');
    stroke(isMatch ? COLORS.match : thrdColor);
    strokeWeight(3);
    
    for (let k = 0; k < targetString.length; k++) {
        let globalPos = step.pos + k;
        if (globalPos >= dnaSequence.length) break;

        let col = globalPos % CONFIG.dnaSegmentLength;
        let row = Math.floor(globalPos / CONFIG.dnaSegmentLength);
        let x = CONFIG.dnaStartX + col * CONFIG.dnaBoxSize;
        let y = CONFIG.dnaStartY + row * CONFIG.dnaRowSpacing;

        rect(x, y, CONFIG.dnaBoxSize, CONFIG.dnaBoxSize, 4);
    }
    
    let firstCol = step.pos % CONFIG.dnaSegmentLength;
    let firstRow = Math.floor(step.pos / CONFIG.dnaSegmentLength);
    let textX = CONFIG.dnaStartX + firstCol * CONFIG.dnaBoxSize;
    let textY = CONFIG.dnaStartY + firstRow * CONFIG.dnaRowSpacing;

    noStroke();
    fill(COLORS.text);
    textAlign(LEFT, BOTTOM);
    let txt = isMatch ? "encontrado" : "analizando";
    text(`Hilo ${step.id} ${txt}: ${targetString}`, textX, textY - 5);
    textAlign(CENTER, CENTER);
}

function drawPatternQueue(stepIndex) {
    let currentY = CONFIG.queueStartY;
    let currentX = CONFIG.queueStartX;
    
    fill(COLORS.text);
    noStroke();
    textAlign(LEFT, CENTER);
    text("Inventario de Patrones:", CONFIG.queueStartX, currentY - 25);
    textAlign(CENTER, CENTER);

    let tempStates = {};
    for (let i = 0; i < patterns.length; i++) tempStates[i] = 'queued';

    for (let i = 0; i <= stepIndex; i++) {
        let log = logData[i];
        if (!log) continue;
        let pId = log.patId;
        if (tempStates[pId] !== 'match') {
            tempStates[pId] = (log.match === 1) ? 'match' : 'missing';
        }
    }

    for (let i = 0; i < patterns.length; i++) {
        let patString = patterns[i].seq;
        let state = tempStates[i];
        
        let patTotalWidth = patString.length * CONFIG.queueBoxSize;
        if (currentX + patTotalWidth > CONFIG.queueLimitX) {
            currentX = CONFIG.queueStartX;
            currentY += CONFIG.queueRowSpacing;
        }
        
        let boxFill, boxStroke;
        if (state === 'queued') {
            boxFill = COLORS.bg;
            boxStroke = COLORS.text + '44';
        } else if (state === 'match') {
            boxFill = COLORS.match + '44';
            boxStroke = COLORS.match;      
        } else if (state === 'missing') {
            boxFill = COLORS.miss + '44';
            boxStroke = COLORS.miss;
        }

        for (let k = 0; k < patString.length; k++) {
            fill(boxFill);
            stroke(boxStroke);
            strokeWeight(2);
            rect(currentX, currentY, CONFIG.queueBoxSize, CONFIG.queueBoxSize, 4);
            
            noStroke();
            fill(COLORS.text);
            text(patString[k], currentX + CONFIG.queueBoxSize/2, currentY + CONFIG.queueBoxSize/2);
            currentX += CONFIG.queueBoxSize;
        }
        currentX += CONFIG.queueSpacing;
    }
}

// =========== CONTROL DE EVENTOS ===========

function togglePlay() {
    if (logData.length === 0) return;
    isPlaying = !isPlaying;
    if(playBtn) playBtn.innerText = isPlaying ? "⏸" : "▶";
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
    if(playBtn) playBtn.innerText = "▶";
}

// =========== CARGA Y PARSEO DEL ARCHIVO .LOG ===========
function loadData(event) {
    const file = event.target.files[0];
    if (!file) return;
    
    const reader = new FileReader();
    reader.onload = function(e) {
        try {
            const data = JSON.parse(e.target.result);
            if(data.dna && data.patterns && data.logs) {
                dnaSequence = data.dna;
                patterns = data.patterns;
                logData = data.logs;
                if(data.mode) mode = data.mode;
                if(data.n_ids) numIds = data.n_ids;
                resetSim();
            } else {
                alert("Error al cargar el archivo. Formato JSON no válido.");
            }
        } catch(err) {
            alert("Error al cargar el archivo. Sintaxis no válida");
        }
    };
    reader.readAsText(file);
}