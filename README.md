# Vk Pong Demo

Simple Pong game demo made after going through vulkan tutorials mainly from vulkan-tutorial.com and https://www.intel.com/content/www/us/en/developer/articles/training/api-without-secrets-introduction-to-vulkan-part-1.html.

<center>
<a href="https://ibb.co/j5WgGb0"><img src="https://i.ibb.co/ZJYWKG4/1.png" alt="1" border="0"></a>
</center>


## Configuration file
In mod dir, there is configuration file (mod.json) in which it is possible to change scaling, positions, speeds and link paths to custom textures for players, ball and background.

	
	{	
		"backgroundTexturePath": "textures/background.jpeg",

		"lhsPlayerTexturePath": "textures/lhsP.png",
		"lhsPlayerHorizontalPos": -0.75,
		"lhsPlayerScaling": [0.03, 0.2],
		"lhsPlayerSpeed": 0.001,

		"rhsPlayerTexturePath": "textures/rhsP.png",
		"rhsPlayerHorizontalPos": 0.75,
		"rhsPlayerScaling": [0.03, 0.2],
		"rhsPlayerSpeed": 0.001,

		"ballTexturePath": "textures/ball.png",
		"ballScaling": [0.04, 0.04],
		"ballSpeed": 0.001,

		"framesInFlight": 1
	}


## Running on linux

To run you need glfw3 windowing library, Vulkan SDK and {fmt} lib. Also C++20 compiler. Then
`./rebuild` -> `./compile` ->`./run`

##
##
##
