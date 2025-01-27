# Hirsch-Water

Code-along practice with HirschDaniel's four-part videos "Coding Fluid Simulation in C". This is a window program using [SDL](https://www.libsdl.org/) where you can generate solid blocks and animated "water". The simulation is based on the article "2D Liquid Simulator With Cellular Automaton in Unity" (https://www.jgallant.com/2d-liquid-simulator-with-cellular-automaton-in-unity/)

## The four-part videos of Hirsch Daniel:
1. https://www.youtube.com/watch?v=DlCJzXdRdNQ
2. https://www.youtube.com/watch?v=oUoAuCUs4Hs
3. https://www.youtube.com/watch?v=pPnEGR56lkA
4. https://www.youtube.com/watch?v=Y4ctqY4FQTg

## Controls

Holding the click on the grid while moving the cursor will generate either a solid block or water depending on which one is selected (the solid block by default). 
You can switch between the solid block and water by pressing the space key.
You can enter/quit a delete mode by pressing the backspace key, allowing to delete the block or water by clicking and dragging over them.

## Screenshot

![image](https://github.com/user-attachments/assets/f998f1e2-1251-4a73-b67e-ccca5f8c3a76)

## Compilation

Open the folder with VS Code, run 'make' on the console then './main', or run the generated `main.exe` in the same folder. Just make sure you have gcc installed and the path in `c_cpp_properties.json` uses your gcc path.

## Comments

I followed along for most of the videos but in episode 4 I was having a segmentation fault problem I did not want to troubleshoot, so I redone part of the code with the help of ChatGPT and DeepSeek, so my code is not 100% like Hirsch's.

For some reason the colors of my water are clearer compared to Hirsch's despite having the exact coloring code. I guess the difference on our simulation code are the reason of that.

Because of the different simulation code, my water looks more fluid, though not totally realistic.

I did not make the part so the water falling down is rendered as a whole block instead of a sliced block because that might need to re-structure the cell struct and change codes here and there. In the Hirsch's code, he directly just made all the blocks to be rendered as full blocks instead of partial ones.

After reading the article closely, I think this simulation was meant to have extra attributes for the struct representing the water cell, adding the direction of the flow for up, down, left and right.

My water flows slowly on the sides; I suppose applying the pressurized water property to the sides, and not only the top direction, would make it more fluid.
