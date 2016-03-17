![Hatched](http://cricklet.github.io/images/hatching.png)

This is a renderer I built as a self-driven final project for COS 526, a graduate level course on graphics. It renders a scene with soft shadows using variance shadow mapping [Donnelly 2006], hemisphere based screen-space ambient occlusion, and real-time hatching [Praun 2001].

I built the renderer completely from scratch in modern C++: I use RAII to handle OpenGL resources, lambda closures to cleanly setup different shader pipelines, shared_ptr for memory management, etc.

You can see a detailed write-up here: http://cricklet.github.io/cos526_final/index.html
