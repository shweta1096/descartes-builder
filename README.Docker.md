# Steps to set Up and run DescartesBuilder within Docker

1. Create a new folder
2. Clone descartes-builder and its related submodules and kedro umbrella inside the folder
3. Copy Dockerfile inside descartes-builder to current directory
4. Open a WSL shell, and navigate to the directory
5. Install x11 by
	sudo apt-get install x11-apps
	sudo apt update && sudo apt install -y xorg x11-xserver-utils
6. Run
	docker build -t descartes_builder:0.1 -t descartes_builder:latest .
7. Run
	docker run -it -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix descartes_builder:0.1
