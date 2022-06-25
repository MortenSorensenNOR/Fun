

// 1D Perlin Noise
let outputSize = 256
let seedPerlinNoise1D = Array.from(Array(outputSize), x => Math.random())
let perlinNoise1D = new Array(outputSize)

let octaveCount = 1
let scalingBias = 2
let mode = 1

// 2D Perlin Noise
let outputWidth = 512
let outputHeight = 512
let seedPerlinNoise2D = Array.from(Array(outputWidth * outputHeight), x => x = Math.random())
let perlinNoise2D = new Array(outputWidth * outputHeight)

// Canvas
// const canvas = document.querySelector('canvas');
// const ctx = canvas.getContext('2d');
let width = /* canvas.width =*/ outputWidth;
let height = /* canvas.height = */ outputHeight;


function generatePerlinNoise1D(count, seed, octaves, bias, output) {
    for (let x = 0; x < count; x++) {
        let noise = 0
        let scale = 1
        let rescaleFactor = 0

        for (let o = 0; o < octaves; o++) {
            let pitch = count >> o
            let sample1 = Math.floor(x / pitch) * pitch
            let sample2 = (sample1 + pitch) % count

            let blendValue = (x - sample1) / pitch

            let sampleLerp = (1 - blendValue) * seed[sample1] + blendValue * seed[sample2]
            noise += sampleLerp * scale
            rescaleFactor += scale
            scale = scale / bias
        }

        output[x] = noise / rescaleFactor
    }
}

function generatePerlinNoise2D(width, height, seed, octaves, bias, output) {
    let start = performance.now()
    for (let x = 0; x < width; x++)  {
        for (let y = 0; y < height; y++) {
            let noise = 0
            let scale = 1
            let rescaleFactor = 0

            for (let o = 0; o < octaves; o++) {
                let pitch = width >> o
                let sampleX1 = Math.floor(x / pitch) * pitch
                let sampleY1 = Math.floor(y / pitch) * pitch

                let sampleX2 = (sampleX1 + pitch) % width
                let sampleY2 = (sampleY1 + pitch) % width

                let blendValueX = (x - sampleX1) / pitch
                let blendValueY = (y - sampleY1) / pitch

                let sampleT = (1 - blendValueX) * seed[sampleY1 * width + sampleX1] + blendValueX * seed[sampleY1 * width + sampleX2]
                let sampleB = (1 - blendValueX) * seed[sampleY2 * width + sampleX1] + blendValueX * seed[sampleY2 * width + sampleX2]

                noise += (blendValueY * (sampleB - sampleT) + sampleT) * scale
                rescaleFactor += scale
                scale = scale / bias
            }

            output[y * width + x] = noise / rescaleFactor
        }
    }
    console.log(performance.now()-start)
}

// function scale (number, inMin, inMax, outMin, outMax) {
//     return (number - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
// }

// function getMax(arr) {
//     let len = arr.length;
//     let max = -Infinity;

//     while (len--) {
//         max = arr[len] > max ? arr[len] : max;
//     }
//     return max;
// }

// function getMin(arr) {
//     let len = arr.length;
//     let min = Infinity;

//     while (len--) {
//         min = arr[len] < min ? arr[len] : min;
//     }
//     return min;
// }

generatePerlinNoise2D(width, height, seedPerlinNoise2D, 10, 2.2, perlinNoise2D)

// let maxPerlinValue2D = getMax(perlinNoise2D)
// let minPerlinValue2D = getMin(perlinNoise2D)
// let colorPallet = ['#003C5F', '#005477', '#009dc4', '#c2b280', '#7CB68E', '#117c13', '#3b510f', '#324f3a', '#57593c', '#828c75']
// let seaLevel = 100
// let heightToColor = {
//     0: 0,
//     1: seaLevel/2,
//     2: seaLevel,
//     3: seaLevel*1.1,
//     4: seaLevel*1.5,
//     5: seaLevel*2,
//     6: 220,
//     7: 240,
//     8: 250,
//     9: 255,
// } 

// function draw() {
//     let imageData = ctx.createImageData(outputWidth, outputHeight)
//     for (let y = 0; y < height; y++) {
//         for (let x = 0; x < width; x++) {
//             let index = (width * y + x) * 4
            
//             let color = perlinNoise2D[width * y + x] * 255
//             let colorIndex = 0
            
//             colorIndex = Math.floor(scale(color, 0, 255, 0, colorPallet.length))

//             let r = parseInt(colorPallet[colorIndex][1] + colorPallet[colorIndex][2], 16),
//                 g = parseInt(colorPallet[colorIndex][3] + colorPallet[colorIndex][4], 16),
//                 b = parseInt(colorPallet[colorIndex][5] + colorPallet[colorIndex][6], 16)
            
//             imageData.data[index] = r
//             imageData.data[index + 1] = g
//             imageData.data[index + 2] = b
//             imageData.data[index + 3] = 255
//         }
//     }

//     ctx.putImageData(imageData, 0, 0)
// }
// requestAnimationFrame(draw)



// THREE JS Terrain Visulization - https://threejs.org/docs/#examples/en/geometries/ConvexGeometry
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera( 60, window.innerWidth / window.innerHeight, 0.1, 1000 );

const renderer = new THREE.WebGLRenderer({ antialias: true });
renderer.setSize( window.innerWidth, window.innerHeight );
document.body.appendChild( renderer.domElement );

camera.position.x = -10
camera.position.y = 15
camera.position.z = 60

// renderer.physicallyCorrectLights = true;

let sphGeo = new THREE.SphereGeometry(20, 8, 6)

let phongMaterial = new THREE.MeshPhongMaterial({
    color: 0x0095DD
});
let sphMesh = new THREE.Mesh(sphGeo, phongMaterial);

let light = new THREE.AmbientLight(0xFFFFFF);
light.position.set(-10, 15, 50);
scene.add(light);

let controls = new OrbitControls( camera, renderer.domElement );
controls.minDistance = 10;
controls.maxDistance = 1000;
controls.maxPolarAngle = Math.PI / 2;
controls.autoRotate = true

let scaleFactor = 200

controls.target.y = perlinNoise2D[ Math.floor(outputWidth / 2 + outputHeight/2 * outputWidth)] * scaleFactor - scaleFactor / 2;
camera.position.y = controls.target.y;
camera.position.x = 0;
camera.position.z = 0;
camera.lookAt(Math.floor(outputWidth/2), Math.floor(outputHeight/2), perlinNoise2D[ Math.floor(outputWidth / 2 + outputHeight/2 * outputWidth)] * scaleFactor - scaleFactor / 2)
controls.update();

// ==== SKYBOX ====
function createPathStrings(filename) {
    const basePath = "/textures/cloudy/";
    const baseFilename = basePath + filename;
    const fileType = ".jpg";
    const sides = ["ft", "bk", "up", "dn", "rt", "lf"];
    const pathStings = sides.map(side => {
        return baseFilename + "_" + side + fileType;
    });
    return pathStings;
}

let skyboxImage = "yellowcloud";
function createMaterialArray(filename) {
    const skyboxImagepaths = createPathStrings(filename);
    const materialArray = skyboxImagepaths.map(image => {
        let texture = new THREE.TextureLoader().load(image);
        
        return new THREE.MeshBasicMaterial({ map: texture, side: THREE.BackSide }); // <---
    });
    return materialArray;
}

let materialArray = createMaterialArray(skyboxImage);
let skyboxGeo = new THREE.BoxGeometry(750, 750, 750);
let skybox = new THREE.Mesh(skyboxGeo, materialArray);
scene.add(skybox);


// ==== Terrain ====
const geometry = new THREE.PlaneGeometry(250, 250, outputWidth - 1, outputHeight - 1);

for (let x = 0; x < outputWidth; x++) {
    for (let y = 0; y < outputHeight; y++) {
        let index = (y * outputWidth + x) * 3
        geometry.attributes.position.array[index + 2] = perlinNoise2D[y * outputWidth + x] * scaleFactor - scaleFactor/2
    }
}

light.position.z = perlinNoise2D[outputHeight/2 * outputWidth + outputWidth] * scaleFactor

var material = new THREE.MeshPhongMaterial({
  color: 0xffffff,
  wireframe: true
});

var plane = new THREE.Mesh(geometry, material);
scene.add(plane);
plane.rotation.x = -Math.PI/2

const fps = document.querySelector('#fps')

function loop(ts) {
    let dt = ts - lastTS

    fps.innerText = `${Math.round(1/(dt/1000) * 100) / 100}`

    renderer.render( scene, camera )
    controls.update()

    lastTS = ts

    requestAnimationFrame(loop)
}
let lastTS = 0
loop()