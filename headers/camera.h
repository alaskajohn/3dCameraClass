// CAMERA CONTROL CLASS for DirectX v9c
//  -These camera tools are used to initialize and control a DirectX camera.
// -------------------------------------------------------------------------
//	Notes:
//  -Requires a "correctly" calculated upVector (perpendicular to the vector 
//    between camera position and camera lookat)
//  -If you initialize or update cameraPosition and/or cameraLookAt without 
//    defining upVector one will be calculated (best guess) for you based on
//    the positive y-axis (the functions are overloaded)
//  -Angles are expected in degrees
//
//	Future Ideas:
//	-Include projection/frustum setup for camera controllig camera lense
// -------------------------------------------------------------------------
//  (C)opyright: John Pile Jr, October 2007
//  http://www.alaskajohn.com
//  May be used for academic purposes with written permission of the author
// -------------------------------------------------------------------------
#ifndef CAMERAH
#define CAMERAH

//#include <C:\\Program Files\\Microsoft DirectX SDK (August 2007)\\include\\d3dx9.h>

class camera
{
public:
	//Camera Administration
	void diagnostics(void);
	void init(IDirect3DDevice9 *, D3DXVECTOR3 location, D3DXVECTOR3 lookAt);
	void init(IDirect3DDevice9 *, D3DXVECTOR3 location, D3DXVECTOR3 lookAt, D3DXVECTOR3 up);
	void Cleanup(void);
	//Camera Position Translation Controls
	void zoom(float value);
	//Camera Position and LookAt Translation Controls
	void moveIn(float value);
	void pan(float horizontal, float vetical); //vertical is similar to lift
	//Camera Rotation Controls
	void pivit(float theat); //same as aircraft yaw
	void tilt(float theta); //same as aircraft pitch
	void roll(float theta);
	//Orbital Controls
	void geoSynchronousOrbit(float theta);
	void polarOrbit(float theta);
	//Manual Camera Updates
	void definePosition(D3DXVECTOR3 newLocation);
	void definePosition(D3DXVECTOR3 newLocation, D3DXVECTOR3 up);
	void defineLookAt(D3DXVECTOR3 newLocation);
	void defineLookAt(D3DXVECTOR3 newLocation, D3DXVECTOR3 up);
private:
	IDirect3DDevice9* Device; 
	D3DXMATRIX View;
	D3DXVECTOR3 cameraPos;
	D3DXVECTOR3 cameraLookAt;
	D3DXVECTOR3 upVector;
	void makeItSo();
	void calculateDefaultUpVector();
};
#endif