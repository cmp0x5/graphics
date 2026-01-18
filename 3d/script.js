const BACKGROUND = "#101010"
const FOREGROUND = "cyan"
const FPS = 60;

console.log(game)
game.width = 800
game.height = 800

const ctx = game.getContext("2d")
console.log(ctx)

function clear() 
{
    ctx.fillStyle = BACKGROUND
    ctx.fillRect(0, 0, game.width, game.height)
}

function point({x, y})
{
    const s = 20;
    ctx.fillStyle = FOREGROUND
    ctx.fillRect(x - s/2, y - s/2, s, s)
}

function screen(p)
{
    return {
        x: (p.x + 1)/2*game.width,
        y: (1 - (p.y + 1)/2)*game.height,
    }
}

function project({x, y, z})
{
    return {
        x: x/z, 
        y: y/z,
    }
}

const vs = [
    {x: 0.5, y: 0.5, z: 0.5},
    {x: -0.5, y: 0.5, z: 0.5},
    {x: 0.5, y: -0.5, z: 0.5},
    {x: -0.5, y: -0.5, z: 0.5},

    {x: 0.5, y: 0.5, z: -0.5},
    {x: -0.5, y: 0.5, z: -0.5},
    {x: 0.5, y: -0.5, z: -0.5},
    {x: -0.5, y: -0.5, z: -0.5},
]

function translate_z({x, y, z}) {
    return {x, y, z: z + dz};
}

function rotate_xz({x, y, z}, angle) {
    const c = Math.cos(angle);
    const s = Math.sin(angle);
    return {
        x: x*c - z*s,
        y,
        z: x*s + z*c,
    }; 
}

let dz = 1;
let angle = 0;

function frame() 
{
    const dt = 1/FPS;
    dz += 1 * dt;
    angle += 2 * Math.PI * dt;
    clear()
    for (const v of vs) {
        point(screen(project(translate_z(rotate_xz(v, angle), dz))))

    }


    setTimeout(frame, 1000/FPS);
}

setTimeout(frame, 1000/FPS);