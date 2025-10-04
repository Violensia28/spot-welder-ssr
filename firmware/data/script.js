function setMode(mode) {
    document.querySelectorAll('.mode-btn').forEach(btn => {
        btn.classList.remove('active');
    });
    event.target.classList.add('active');
    
    if(mode === 'manual') {
        document.getElementById('manualSettings').style.display = 'block';
        document.getElementById('autoSettings').style.display = 'none';
    } else {
        document.getElementById('manualSettings').style.display = 'none';
        document.getElementById('autoSettings').style.display = 'block';
    }
    
    fetch('/settings?mode=' + mode);
    document.getElementById('currentMode').textContent = mode.toUpperCase();
}

function updateSliders() {
    document.getElementById('weldTimeValue').textContent = document.getElementById('weldTime').value;
    document.getElementById('pulseCountValue').textContent = document.getElementById('pulseCount').value;
    
    const time = document.getElementById('weldTime').value;
    const pulses = document.getElementById('pulseCount').value;
    fetch('/settings?time=' + time + '&pulses=' + pulses);
}

async function startWelding() {
    document.getElementById('weldStatus').textContent = 'WELDING...';
    document.getElementById('weldStatus').style.color = '#FF416C';
    
    try {
        await fetch('/weld');
        // Beep sound simulation
        beepSound();
    } catch (error) {
        console.error('Welding error:', error);
    }
    
    document.getElementById('weldStatus').textContent = 'READY';
    document.getElementById('weldStatus').style.color = '#4CAF50';
}

function emergencyStop() {
    document.getElementById('weldStatus').textContent = 'EMERGENCY STOP';
    document.getElementById('weldStatus').style.color = '#FF0000';
    alert('EMERGENCY STOP ACTIVATED!');
}

function beepSound() {
    // Simulate beep using Web Audio
    const audioContext = new (window.AudioContext || window.webkitAudioContext)();
    const oscillator = audioContext.createOscillator();
    const gainNode = audioContext.createGain();
    
    oscillator.connect(gainNode);
    gainNode.connect(audioContext.destination);
    
    oscillator.frequency.value = 800;
    oscillator.type = 'sine';
    gainNode.gain.setValueAtTime(0.3, audioContext.currentTime);
    gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.5);
    
    oscillator.start(audioContext.currentTime);
    oscillator.stop(audioContext.currentTime + 0.5);
}

// Initialize
document.getElementById('weldTime').addEventListener('input', updateSliders);
document.getElementById('pulseCount').addEventListener('input', updateSliders);
updateSliders();
