
console.log("Injector - The JavaScript has been injected 🤖")


var audioContext = null;
var audioWorkerNode = null;
var microphoneInputStream = null;

function toggleMicrophone(ev){

    if(audioContext){
        //stop recording
        //var button_text = 'Start recording';
        //ev.srcElement.innerHTML = button_text;

        microphoneInputStream.getAudioTracks().forEach(track => {
            track.stop();
        });
        
        console.log(audioWorkerNode);

        audioContext.close();
        audioContext = null;
    }else{
        //var button_text = 'Stop recording <svg width="24" height="24"><circle fill="#ff0000" stroke="none" cx="12" cy="12" r="12"><animate attributeName="opacity" dur="1s" values="0;1;0" repeatCount="indefinite" begin="0.1" /></circle></svg>'  
        //ev.srcElement.innerHTML = button_text;
        
        console.log("Attempting to start using the microphone");
        navigator.mediaDevices.getUserMedia({ audio: true })
          .then(async function(stream) {
            audioContext = new AudioContext();
            
            microphoneInputStream = stream;

            await audioContext.audioWorklet.addModule("js/pitch.detector.audioworklet.js");

            audioWorkerNode = new AudioWorkletNode(audioContext, "pitch-detector-audio-processor",{
              numberOfOutputs: 0,
              numberOfInputs: 1,
              processorOptions: {
                sampleRate: audioContext.sampleRate,
              }});

            audioWorkerNode.port.onmessage = (event) => {
                handlePitchEstimates(event.data)
            };

            const microphoneNode = audioContext.createMediaStreamSource(stream);
            microphoneNode.connect(audioWorkerNode);
            //audioWorkerNode.connect(audioContext.destination);
            //audioWorkerNode.connect(audioContext.destination);
            if (audioContext.start) {
              audioContext.start();
            } else if (audioContext.resume) {
              audioContext.resume();
            }

          })
          .catch(function(error) {
            console.log("Could not access microphone",error)
          });
    }
}



const transpose = 50;//0-50; //cents
var follower;
var currentX=10;
var currentY=10;
const cursor_movement = 5;

// Function to create the emoji or SVG element
function createCursorFollower() {
    follower = document.createElement('div');
    follower.innerHTML = '🪈';
    follower.style.position = 'absolute';
    follower.style.fontSize = '40px';
    follower.style.zIndex = '1000';
    follower.style.pointerEvents = 'none'; // Ensures the element doesn't interfere with other Cursor events
    document.body.appendChild(follower);
}

function createButton() {
  let button = document.createElement('button');
  
  button.innerHTML = 'Start Pointer';
  button.style.position = 'absolute';
  button.style.top = '100px';
  button.style.left = '100px';
  
  button.id = 'startButton'

  document.body.appendChild(button);
}

// Function to update the position of the element
function updateCursorFollowerPosition( x, y) {
    follower.style.left = (x-38) + 'px';
    follower.style.top = y + 'px';
}

document.addEventListener('keydown', function(event) {
  switch (event.key) {
      case 'ArrowUp':
          currentY = currentY - cursor_movement;
          break;
      case 'ArrowDown':
          currentY = currentY + cursor_movement;
          break;
      case 'ArrowLeft':
          currentX = currentX - cursor_movement;
          break;
      case 'ArrowRight':
          currentX = currentX + cursor_movement;
          break;
      case 'c':
      case 'C':
          //click
          let element = document.elementFromPoint(currentX, currentY)
          if(element != null) element.click();
          break;
      default:
          // Handle other keys if needed
          break;
  }
  updateCursorFollowerPosition(currentX,currentY);
});

function handlePitchEstimates(estimation){

  const midi_cents = estimation['pitch_in_cents']
  
  const midi_cents_transposed = midi_cents + transpose;
  const midi_note = Math.round(midi_cents_transposed/ 100);
  const midi_pitch_class = midi_note % 12;
  
  //console.log(midi_note,midi_pitch_class,estimation)
  console.log(midi_note,midi_pitch_class,estimation)

  switch (midi_pitch_class) {
  case 11:
    //B = clik
    let element = document.elementFromPoint(currentX, currentY)
    if(element != null) element.click();
    break;
  case 0:  
    // C = down
    currentY = currentY + cursor_movement;break;
  case 2: 
    //D = right
    currentX = currentX + cursor_movement;break;
  case 4: 
    //E = left 
    currentX = currentX - cursor_movement;break;
  case 5:
    //F = up
    currentY = currentY - cursor_movement;break;
  }
  updateCursorFollowerPosition(currentX,currentY);
}

window.onload = function(){

  createButton();
  document.getElementById('startButton').addEventListener('click', async (ev) => {
    toggleMicrophone(ev);
  });

  // Initialize the mouse follower
  createCursorFollower();
  document.addEventListener('mousemove', (event) => {
        currentX = event.clientX;
        currentY = event.clientY;
        updateCursorFollowerPosition(currentX, currentY);
  });

  toggleMicrophone(null);
}


