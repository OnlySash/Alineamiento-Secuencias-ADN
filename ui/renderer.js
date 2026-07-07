// === CFG GRAFICA GLOBAL (Optimizada para 1366x768) ===
const config = {
  fps: 3,
  canvasWidth: 960,
  minCanvasHeight: 400,

  // Cuadrícula de ADN
  dna: {
    startX: 40,
    startY: 60,
    boxSize: 30,
    segmentLength: 28,
    rowSpacing: 60,
  },

  // Cuadrícula de patrones
  pattern: {
    startX: 40,
    startY: 400,
    limitX: 920,
    boxSize: 22,
    rowSpacing: 40,
    spacing: 15,
  },
};

// === PALETAS DE COLOR ===
const darkTheme = {
  bg: "#1e1e2e", text: "#cdd6f4", match: "#a6e3a1", miss: "#f38ba8",
  threads: ["#89b4fa", "#f9e2af", "#cba6f7", "#fab387", "#a6adc8"],
  nucleotides: { 'A': '#a6e3a1', 'T': '#f38ba8', 'C': '#f9e2af', 'G': '#89b4fa' }
};

const lightTheme = {
  bg: "#eef1f5", text: "#4c4f69", match: "#40a02b", miss: "#d20f39",
  threads: ["#1e66f5", "#df8e1d", "#8839ef", "#fe640b", "#7c7f93"],
  nucleotides: { 'A': '#40a02b', 'T': '#d20f39', 'C': '#df8e1d', 'G': '#1e66f5' }
};

let colors = darkTheme;

// === DATOS DE UI ===
const UIData = {
  dna: "", patterns: [], logs: [], mode: "", totalThreads: 0,
  currentStep: 0, isPlaying: false,
  get currentLog() { return this.logs[this.currentStep - 1] || null; },
};

let playBtn;

// === P5.JS ===
function setup() {
  let canvas = createCanvas(config.canvasWidth, config.minCanvasHeight);
  let container = document.getElementById("canvas-container");
  if (container) canvas.parent("canvas-container");

  frameRate(config.fps); 
  textAlign(CENTER, CENTER);
  textSize(14);
  playBtn = document.getElementById("playBtn");
}

function draw() {
  background(colors.bg);
  if (UIData.dna.length === 0) return;

  drawDNA();
  handlePlayback();

  const activeLog = UIData.currentLog;
  if (activeLog) {
    drawScanner(activeLog);
  }
  drawPatternQueue();
}

// === CALCULO DINAMICO DEL LAYOUT ===
function updateLayout() {
  if (UIData.dna.length === 0) return;

  let dnaRows = Math.ceil(UIData.dna.length / config.dna.segmentLength);
  let dnaBottomY = config.dna.startY + (dnaRows * config.dna.rowSpacing);

  config.pattern.startY = dnaBottomY + 40;

  let currentX = config.pattern.startX;
  let currentY = config.pattern.startY;
  
  for (let i = 0; i < UIData.patterns.length; i++) {
    let pWidth = UIData.patterns[i].seq.length * config.pattern.boxSize;
    if (currentX + pWidth > config.pattern.limitX) {
      currentX = config.pattern.startX;
      currentY += config.pattern.rowSpacing;
    }
    currentX += pWidth + config.pattern.spacing;
  }

  let finalRequiredHeight = currentY + config.pattern.boxSize + 40; 

  let newHeight = Math.max(config.minCanvasHeight, finalRequiredHeight);
  resizeCanvas(config.canvasWidth, newHeight);
}

// === FUNCIONES DE DIBUJO ===
function getGridCoords(index) {
  let col = index % config.dna.segmentLength;
  let row = Math.floor(index / config.dna.segmentLength);
  return {
    x: config.dna.startX + col * config.dna.boxSize,
    y: config.dna.startY + row * config.dna.rowSpacing,
  };
}

function drawDNA() {
  strokeWeight(1);
  for (let i = 0; i < UIData.dna.length; i++) {
    let pos = getGridCoords(i);
    let nuc = UIData.dna[i];
    let nucColorHex = colors.nucleotides[nuc] || colors.text;
    let nucColor = color(nucColorHex);
    
    let boxFill = color(nucColorHex);
    boxFill.setAlpha(30); 
    
    fill(boxFill);
    stroke(colors.text);
    rect(pos.x, pos.y, config.dna.boxSize, config.dna.boxSize, 4);

    push();
    noStroke();
    fill(nucColor); 
    text(nuc, pos.x + config.dna.boxSize / 2, pos.y + config.dna.boxSize / 2);
    pop();
  }
}

function drawScanner(logEntry) {
  let pattern = UIData.patterns[logEntry.patId];
  if (!pattern || !pattern.seq) return;

  let sequence = pattern.seq;
  let isMatch = logEntry.match === 1;

  let threadHex = colors.threads[logEntry.id % colors.threads.length];
  let boxFill = color(isMatch ? colors.match : threadHex);
  boxFill.setAlpha(isMatch ? 170 : 68);
  let boxStroke = color(isMatch ? colors.match : threadHex);

  fill(boxFill);
  stroke(boxStroke);
  strokeWeight(3);

  for (let k = 0; k < sequence.length; k++) {
    let globalIndex = logEntry.pos + k;
    if (globalIndex >= UIData.dna.length) break;
    let pos = getGridCoords(globalIndex);
    rect(pos.x, pos.y, config.dna.boxSize, config.dna.boxSize, 4);
  }

  let startPos = getGridCoords(logEntry.pos);
  push();
  noStroke();
  fill(colors.text);
  textAlign(LEFT, BOTTOM);
  textSize(12); 
  let statusText = isMatch ? "¡Encontrado!" : "Buscando...";
  text(`[T_${logEntry.id}] ${statusText}: ${sequence}`, startPos.x, startPos.y - 4);
  pop();
}

function drawPatternQueue() {
  let currentX = config.pattern.startX;
  let currentY = config.pattern.startY;
  let patternStatus = new Array(UIData.patterns.length).fill("queued");

  for (let i = 0; i < UIData.currentStep; i++) {
    let log = UIData.logs[i];
    if (log && patternStatus[log.patId] !== "match") {
      patternStatus[log.patId] = log.match === 1 ? "match" : "missing";
    }
  }

  let matchesFound = patternStatus.filter(status => status === "match").length;

  push();
  fill(colors.text);
  noStroke();
  textAlign(LEFT, CENTER);
  text("Patrones:", config.pattern.startX, currentY - 20);
  
  textAlign(RIGHT, CENTER);
  textSize(15);
  //fill(colors.match);
  text(`Coincidencias: ${matchesFound} / ${UIData.patterns.length}`, config.canvasWidth - 40, config.pattern.startY - 20);
  pop();

  for (let pIndex = 0; pIndex < UIData.patterns.length; pIndex++) {
    let sequence = UIData.patterns[pIndex].seq;
    let status = patternStatus[pIndex];

    let pTotalWidth = sequence.length * config.pattern.boxSize;
    if (currentX + pTotalWidth > config.pattern.limitX) {
      currentX = config.pattern.startX;
      currentY += config.pattern.rowSpacing;
    }

    let boxFill = color(colors.bg);
    let boxStroke = color(100, 100, 100, 100); 

    if (status === "match") {
      boxFill = color(colors.match);
      boxFill.setAlpha(68);
      boxStroke = color(colors.match);
    } else if (status === "missing") {
      boxFill = color(colors.miss);
      boxFill.setAlpha(68);
      boxStroke = color(colors.miss);
    }

    for (let k = 0; k < sequence.length; k++) {
      fill(boxFill);
      stroke(boxStroke);
      strokeWeight(2);
      rect(currentX, currentY, config.pattern.boxSize, config.pattern.boxSize, 3);

      let nucColor = colors.nucleotides[sequence[k]] || colors.text;
      push();
      noStroke();
      fill(nucColor);
      textSize(13);
      text(sequence[k], currentX + config.pattern.boxSize / 2, currentY + config.pattern.boxSize / 2);
      pop();

      currentX += config.pattern.boxSize;
    }
    currentX += config.pattern.spacing;
  }
}

// === INTERACCIONES NUEVAS ===
function updateFPS() {
  let val = document.getElementById("fpsSlider").value;
  config.fps = parseInt(val);
  document.getElementById("fpsLabel").innerText = config.fps + " FPS";
  frameRate(config.fps);
}

function toggleTheme() {
  let isLight = document.getElementById("themeToggle").checked;
  colors = isLight ? lightTheme : darkTheme;
  document.body.classList.toggle("light-mode", isLight);
  document.getElementById("theme-icon").innerText = isLight ? "☀️" : "🌙";
}

// === REPRODUCCIÓN ===
function handlePlayback() {
  if (UIData.isPlaying && UIData.logs.length > 0) {
    if (UIData.currentStep < UIData.logs.length) {
      UIData.currentStep++;
    } else {
      UIData.isPlaying = false;
      updatePlayButton();
    }
  }
}
function togglePlay() {
  if (UIData.logs.length === 0) return;
  if (UIData.currentStep >= UIData.logs.length) UIData.currentStep = 0;
  UIData.isPlaying = !UIData.isPlaying;
  updatePlayButton();
}
function stepForward() { UIData.isPlaying = false; if (UIData.currentStep < UIData.logs.length) UIData.currentStep++; updatePlayButton(); }
function stepBackward() { UIData.isPlaying = false; if (UIData.currentStep > 0) UIData.currentStep--; updatePlayButton(); }
function resetSim() { UIData.currentStep = 0; UIData.isPlaying = false; updatePlayButton(); }
function updatePlayButton() { if (playBtn) playBtn.innerText = UIData.isPlaying ? "⏸" : "▶"; }

// === CARGA DE DATOS ===
async function runSimulation() {
    const runBtn = document.getElementById('runBtn');
    runBtn.innerText = "Ejecutando en C... ⏳";
    runBtn.disabled = true;

    const params = new URLSearchParams({
        m: document.getElementById('sim-mode').value,
        n: document.getElementById('sim-n').value,
        k: document.getElementById('sim-k').value,
        l: document.getElementById('sim-l').value,
        t: document.getElementById('sim-t').value
    });

    try {
        const response = await fetch(`/run-simulation?${params.toString()}`);
        if (!response.ok) throw new Error("Error en C");
        
        const data = await response.json();
        UIData.dna = data.dna;
        UIData.patterns = data.patterns;
        UIData.logs = data.logs;
        UIData.mode = data.mode;
        
        updateLayout(); 
        resetSim();
        togglePlay(); 
    } catch (err) {
        alert("Error al ejecutar.");
    } finally {
        runBtn.innerText = "Lanzar 🚀";
        runBtn.disabled = false;
    }
}