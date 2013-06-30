// CAMERA CONTROL CLASS for DirectX v9c
//	See comments in header file, camera.h for details
// -------------------------------------------------------------------------
//  (C)opyright: John Pile Jr, October 2007
//  http://www.alaskajohn.com
//  May be used for academic purposes with written permission of the author
// -------------------------------------------------------------------------
#include "DXUT.h"
#include "..\headers\camera.h"
#include "..\headers\output.h"		//used for output


void camera::init(IDirect3DDevice9 * d, D3DXVECTOR3 location, D3DXVECTOR3 lookAt)
{
	Device = d;
	cameraPos		=	location;
	cameraLookAt	=	lookAt;
	calculateDefaultUpVector();
	makeItSo();
};
void camera::init(IDirect3DDevice9 * d, D3DXVECTOR3 location, D3DXVECTOR3 lookAt, D3DXVECTOR3 up)
{
	Device = d;
	cameraPos		=	location;
	cameraLookAt	=	lookAt;
	upVector		=	up;
	makeItSo();
};

void camera::Cleanup(void)
{
	//nothing
};

void camera::definePosition(D3DXVECTOR3 newLocation)
{
	cameraPos = newLocation;
	calculateDefaultUpVector();
	makeItSo();
};

void camera::definePosition(D3DXVECTOR3 newLocation, D3DXVECTOR3 up)
{
	cameraPos = newLocation;
	upVector = up;
	makeItSo();
};

void camera::defineLookAt(D3DXVECTOR3 newLocation)
{
	cameraLookAt = newLocation;
	calculateDefaultUpVector();
	makeItSo();
};

void camera::defineLookAt(D3DXVECTOR3 newLocation, D3DXVECTOR3 up)
{
	cameraLookAt = newLocation;
	upVector = up;
	makeItSo();
};

void camera::calculateDefaultUpVector(void)
{
	//The theory... find the 'correct' upVector corresponding to positive Y axis being "up"
	//I left my "plan of attacks" in the comments for entertainment value.  Plan B won out in the end.

	//--------------------------------------------------------------------------------------------
	//This is Plan B...
	//NOTE: with all the theories I battered about... this one is totally wack, but seems to work fine
	//  1) Normalize the lookAtVector (the vector from camera position to camera "look at"
	//  2) When rotated 90 degrees, the length of the vector projected onto the x-z axis will be the new y length
	//	3) The x and z vectors will keep the same relative value (when rotated, only the length of the projection changes)
	//	4) However, if y was already positive, then the x-z projection is now in the opposite quadrant

	D3DXVECTOR3 lookAtVector = cameraPos - cameraLookAt;
	//NORMALIZE
	float length = sqrt( (lookAtVector[0]*lookAtVector[0])
						+(lookAtVector[1]*lookAtVector[1])
						+(lookAtVector[2]*lookAtVector[2]) );


	lookAtVector[0] = lookAtVector[0] / length;
	lookAtVector[1] = lookAtVector[1] / length;
	lookAtVector[2] = lookAtVector[2] / length;

	D3DXVECTOR3 newVector;
	//length of the projection onto the x-z axis will be the length in the z direction of new vector
	newVector[1] = sqrt(lookAtVector[0]*lookAtVector[0]+lookAtVector[2]*lookAtVector[2]);
 
	//The x and z vectors are then fractional components of their previous values
	//except that if y was positive, then x and z change signs
	float percentDif = lookAtVector[1]/newVector[1];
	newVector[0] = percentDif * lookAtVector[0];
	newVector[2] = percentDif * lookAtVector[2];
	if (lookAtVector[1] > 0) {
		newVector[0] = -newVector[0];
		newVector[2] = -newVector[2];
	};

	upVector = newVector;
	//This actually works!! (with limitted testing.. should test with worst case of lookAtVector = (0,1,0)


	//--------------------------------------------------------------------------------------------
	//However my first attempt was... Plan A
	//How?  1) Find the plane paralell to the lookAtVector (in otherwords, normalVector = lookAtVector
	//			and passes through the cameraPos)
	//		2) Find the unitVector on the plane with max Y Value
	//Should work as long as lookAtVector != (0,1,0)
	//Dot Product of lookAtVector and cameraPosition 
/*
	float dot = cameraPos[0] * lookAtVector[0] 
			  + cameraPos[1] * lookAtVector[1]
			  + cameraPos[2] * lookAtVector[2];
*/
	//set normal (lookAtVector) equal to dot product and solve for max y of unit vector
	// We know: lookAtVector[0]x + lookAtVector[1]y + lookAtVector[2]z = dot;
	// And:		x^2 + y^2 + z^2 = 1
	
	//--------------------------------------------------------------------------------------------
    //Plan C was to use resultant z value from Plan B and equations from Plan A to find x and z values.
	//Equation of plane is Ax+By+Cz=D
	//  but we found y is now a contstant K
	//  which becomes Ax + Cz = D/BK, and solving for z = 1/C(D/BK - Ax)
	//And we know x^2 + y^2 +z^2 = 1
	//  which becomes x^2 + z^2 = (1-K) , and solving for x = sqrt( (1-K) - z^2 )
	//Now we have two equations and two unknowns, and substituting eq2 into eq1 we have:
	//  z = sqrt( (1-K) - sqr(1/C(D/BK - Ax)) )
	// (which should have the ...

	//--------------------------------------------------------------------------------------------
	//Plan D:
    //Or, take the dot product of the lookAtVector and the positive X axis to determine theta
			 
};

void camera::diagnostics(void)
{
	char my_temp_buffer[128];

	output_log("*************************************************************");
	output_log("Camera Diagnostics Report:");
	sprintf_s(my_temp_buffer, "cameraPos:    %f, %f, %f", cameraPos[0], cameraPos[1], cameraPos[2] );
	output_log(my_temp_buffer);
	sprintf_s(my_temp_buffer, "cameraLookAt: %f, %f, %f", cameraLookAt[0], cameraLookAt[1], cameraLookAt[2] );
	output_log(my_temp_buffer);
	sprintf_s(my_temp_buffer, "upVector:     %f, %f, %f", upVector[0], upVector[1], upVector[2] );
	output_log(my_temp_buffer);
	output_log("-------------------------------------------------------------");

};

void camera::roll(float theta)
{
	//Camera Roll:
	//Assumption: Camera Position annd Camera Lookat remain unchanged, but the upVector is modified through rotation
	//Plan of Attack:	Find new upVector by rotating x degrees around the lookAtVector axis

	D3DXVECTOR3 lookAtVector = cameraPos - cameraLookAt;
	D3DXVECTOR3 rotateAroundVector = lookAtVector;


	//Now find the new point of cameraLookAt using quaternians: Rotate theta around "rotateAroundVector" at point cameraPos
	//q = cos(qTheta) + (i)sin(qTheta)
	//I(i,j,k) is rotateAroundVector[0,1,2]
	//So:	T^-1 * q * T * p * q^-1 

	float qTheta = theta/2;
	D3DXVECTOR3 Pt = upVector;

	float modA =  sqrt(rotateAroundVector[0]*rotateAroundVector[0]
						+rotateAroundVector[1]*rotateAroundVector[1]
						+rotateAroundVector[2]*rotateAroundVector[2]);
	float qSIN = sin(qTheta*D3DX_PI/180)/modA;

	float qREAL = cos(qTheta*D3DX_PI/180);
	float qI = rotateAroundVector[0] * qSIN;
	float qJ = rotateAroundVector[1] * qSIN;
	float qK = rotateAroundVector[2] * qSIN;

	float pREAL = 0;
	float pI = Pt[0];
	float pJ = Pt[1];
	float pK = Pt[2];

	float qiREAL = qREAL;
	float qiI = -qI;
	float qiJ = -qJ;
	float qiK = -qK;

	D3DXMATRIX q, p, qi;
	q = D3DXMATRIX(	qREAL, -qI,		-qJ,	-qK,
					qI,		qREAL,	-qK,	qJ,
					qJ,		qK,		qREAL,	-qI,
					qK,		-qJ,	qI,		qREAL		);

	p = D3DXMATRIX(	pREAL, -pI,		-pJ,	-pK,
					pI,		pREAL,	-pK,	pJ,
					pJ,		pK,		pREAL,	-pI,
					pK,		-pJ,	pI,		pREAL		);

	qi = D3DXMATRIX(qiREAL, 0,		0,		0,
					qiI,	0,		0,		0,
					qiJ,	0,		0,		0,
					qiK,	0,		0,		0		);

	D3DXMATRIX resultantMatrix;
	resultantMatrix = q * p * qi;
	D3DXVECTOR3 resultantPoint(resultantMatrix(1,0),resultantMatrix(2,0),resultantMatrix(3,0));
	upVector = resultantPoint;

	makeItSo();

};

void camera::pivit(float theta)
{
	//Camera Pivit (same as Yaw of an aircraft):
	//Assumption: Camera Position remains unchanged, but the cameraLookAt is modified through rotation
	//Plan of Attack:	Find new cameraLookAt by rotating x degrees around the upVector axis

	D3DXVECTOR3 rotateAroundVector = upVector;

	//Now find the new point of cameraLookAt using quaternians: Rotate theta around "rotateAroundVector" at point cameraPos
	//q = cos(qTheta) + (i)sin(qTheta)
	//I(i,j,k) is rotateAroundVector[0,1,2]
	//So:	T^-1 * q * T * p * q^-1 

	float qTheta = theta/2;
	D3DXVECTOR3 Pt = cameraLookAt - cameraPos;

	float modA =  sqrt(rotateAroundVector[0]*rotateAroundVector[0]
						+rotateAroundVector[1]*rotateAroundVector[1]
						+rotateAroundVector[2]*rotateAroundVector[2]);
	float qSIN = sin(qTheta*D3DX_PI/180)/modA;

	float qREAL = cos(qTheta*D3DX_PI/180);
	float qI = rotateAroundVector[0] * qSIN;
	float qJ = rotateAroundVector[1] * qSIN;
	float qK = rotateAroundVector[2] * qSIN;

	float pREAL = 0;
	float pI = Pt[0];
	float pJ = Pt[1];
	float pK = Pt[2];

	float qiREAL = qREAL;
	float qiI = -qI;
	float qiJ = -qJ;
	float qiK = -qK;

	D3DXMATRIX q, p, qi;
	q = D3DXMATRIX(	qREAL, -qI,		-qJ,	-qK,
					qI,		qREAL,	-qK,	qJ,
					qJ,		qK,		qREAL,	-qI,
					qK,		-qJ,	qI,		qREAL		);

	p = D3DXMATRIX(	pREAL, -pI,		-pJ,	-pK,
					pI,		pREAL,	-pK,	pJ,
					pJ,		pK,		pREAL,	-pI,
					pK,		-pJ,	pI,		pREAL		);

	qi = D3DXMATRIX(qiREAL, 0,		0,		0,
					qiI,	0,		0,		0,
					qiJ,	0,		0,		0,
					qiK,	0,		0,		0		);

	D3DXMATRIX resultantMatrix;
	resultantMatrix = q * p * qi;
	D3DXVECTOR3 resultantPoint(resultantMatrix(1,0),resultantMatrix(2,0),resultantMatrix(3,0));
	cameraLookAt = resultantPoint + cameraPos;

	makeItSo();
};

void camera::geoSynchronousOrbit(float theta)
{
	//GeoSynchronous Orbit: ie, Rotate cameraPosition around upVector at the cameraLookAt

	D3DXVECTOR3 rotateAroundVector = upVector;

	//Good Old Quaternians...

	float qTheta = theta/2;
	D3DXVECTOR3 Pt = cameraPos - cameraLookAt;

	float modA =  sqrt(rotateAroundVector[0]*rotateAroundVector[0]
						+rotateAroundVector[1]*rotateAroundVector[1]
						+rotateAroundVector[2]*rotateAroundVector[2]);
	float qSIN = sin(qTheta*D3DX_PI/180)/modA;

	float qREAL = cos(qTheta*D3DX_PI/180);
	float qI = rotateAroundVector[0] * qSIN;
	float qJ = rotateAroundVector[1] * qSIN;
	float qK = rotateAroundVector[2] * qSIN;

	float pREAL = 0;
	float pI = Pt[0];
	float pJ = Pt[1];
	float pK = Pt[2];

	float qiREAL = qREAL;
	float qiI = -qI;
	float qiJ = -qJ;
	float qiK = -qK;

	D3DXMATRIX q, p, qi;
	q = D3DXMATRIX(	qREAL, -qI,		-qJ,	-qK,
					qI,		qREAL,	-qK,	qJ,
					qJ,		qK,		qREAL,	-qI,
					qK,		-qJ,	qI,		qREAL		);

	p = D3DXMATRIX(	pREAL, -pI,		-pJ,	-pK,
					pI,		pREAL,	-pK,	pJ,
					pJ,		pK,		pREAL,	-pI,
					pK,		-pJ,	pI,		pREAL		);

	qi = D3DXMATRIX(qiREAL, 0,		0,		0,
					qiI,	0,		0,		0,
					qiJ,	0,		0,		0,
					qiK,	0,		0,		0		);

	D3DXMATRIX resultantMatrix;
	resultantMatrix = q * p * qi;
	D3DXVECTOR3 resultantPoint(resultantMatrix(1,0),resultantMatrix(2,0),resultantMatrix(3,0));
	cameraPos = resultantPoint + cameraLookAt;

	makeItSo();
};

void camera::polarOrbit(float theta)
{
	//GeoSynchronous Orbit: ie, Rotate cameraPosition around upVector at the cameraLookAt

	D3DXVECTOR3 lookAtVector = cameraPos - cameraLookAt;
	D3DXVECTOR3 rotateAroundVector;

	//Calculate Cross Product: lookAtVector X upVector
	rotateAroundVector[0] = lookAtVector[1] * upVector[2] - lookAtVector[2] * upVector[1];
    rotateAroundVector[1] = lookAtVector[2] * upVector[0] - lookAtVector[0] * upVector[2];
    rotateAroundVector[2] = lookAtVector[0] * upVector[1] - lookAtVector[1] * upVector[0];	

	//Good Old Quaternians...
	float qTheta = theta/2;
	D3DXVECTOR3 Pt = cameraPos - cameraLookAt;

	float modA =  sqrt(rotateAroundVector[0]*rotateAroundVector[0]
						+rotateAroundVector[1]*rotateAroundVector[1]
						+rotateAroundVector[2]*rotateAroundVector[2]);
	float qSIN = sin(qTheta*D3DX_PI/180)/modA;

	float qREAL = cos(qTheta*D3DX_PI/180);
	float qI = rotateAroundVector[0] * qSIN;
	float qJ = rotateAroundVector[1] * qSIN;
	float qK = rotateAroundVector[2] * qSIN;

	float pREAL = 0;
	float pI = Pt[0];
	float pJ = Pt[1];
	float pK = Pt[2];

	float qiREAL = qREAL;
	float qiI = -qI;
	float qiJ = -qJ;
	float qiK = -qK;

	D3DXMATRIX q, p, qi;
	q = D3DXMATRIX(	qREAL, -qI,		-qJ,	-qK,
					qI,		qREAL,	-qK,	qJ,
					qJ,		qK,		qREAL,	-qI,
					qK,		-qJ,	qI,		qREAL		);

	p = D3DXMATRIX(	pREAL, -pI,		-pJ,	-pK,
					pI,		pREAL,	-pK,	pJ,
					pJ,		pK,		pREAL,	-pI,
					pK,		-pJ,	pI,		pREAL		);

	qi = D3DXMATRIX(qiREAL, 0,		0,		0,
					qiI,	0,		0,		0,
					qiJ,	0,		0,		0,
					qiK,	0,		0,		0		);

	D3DXMATRIX resultantMatrix;
	resultantMatrix = q * p * qi;
	D3DXVECTOR3 resultantPoint(resultantMatrix(1,0),resultantMatrix(2,0),resultantMatrix(3,0));
	cameraPos = resultantPoint + cameraLookAt;

	//Do it again for upVector: but this only works if upVector was defined correctly
	pREAL = 0;
	pI = upVector[0];
	pJ = upVector[1];
	pK = upVector[2];

	p = D3DXMATRIX(	pREAL, -pI,		-pJ,	-pK,
					pI,		pREAL,	-pK,	pJ,
					pJ,		pK,		pREAL,	-pI,
					pK,		-pJ,	pI,		pREAL		);

	resultantMatrix = q * p * qi;

	D3DXVECTOR3 resultantPoint2(resultantMatrix(1,0),resultantMatrix(2,0),resultantMatrix(3,0));
	upVector = resultantPoint2;
	makeItSo();
};


void camera::tilt(float theta)
{
	//Camera Tilt (same as Pitch of an aircraft):
	//Assumption: Camera Position remains unchanged, but the cameraLookAt is modified through rotation
	//Plan of Attack:	Find new cameraLookAt by rotating x degrees around the axis
	//					perpendicular to the plane defined by the upVector  and the vector between
	//				    the cameraPosition and the cameraLookAt, at the cameraPosition 

	D3DXVECTOR3 lookAtVector = cameraPos - cameraLookAt;
	D3DXVECTOR3 rotateAroundVector;

	//Calculate Cross Product: lookAtVector X upVector
	rotateAroundVector[0] = lookAtVector[1] * upVector[2] - lookAtVector[2] * upVector[1];
    rotateAroundVector[1] = lookAtVector[2] * upVector[0] - lookAtVector[0] * upVector[2];
    rotateAroundVector[2] = lookAtVector[0] * upVector[1] - lookAtVector[1] * upVector[0];	

	//Now find the new point of cameraLookAt using quaternians: Rotate theta around "rotateAroundVector" at point cameraPos
	//q = cos(qTheta) + (i)sin(qTheta)
	//I(i,j,k) is rotateAroundVector[0,1,2]
	//So:	T^-1 * q * T * p * q^-1 

	float qTheta = theta/2;
	D3DXVECTOR3 Pt = cameraLookAt - cameraPos;

	float modA =  sqrt(rotateAroundVector[0]*rotateAroundVector[0]
						+rotateAroundVector[1]*rotateAroundVector[1]
						+rotateAroundVector[2]*rotateAroundVector[2]);
	float qSIN = sin(qTheta*D3DX_PI/180)/modA;

	float qREAL = cos(qTheta*D3DX_PI/180);
	float qI = rotateAroundVector[0] * qSIN;
	float qJ = rotateAroundVector[1] * qSIN;
	float qK = rotateAroundVector[2] * qSIN;

	float pREAL = 0;
	float pI = Pt[0];
	float pJ = Pt[1];
	float pK = Pt[2];

	float qiREAL = qREAL;
	float qiI = -qI;
	float qiJ = -qJ;
	float qiK = -qK;

	D3DXMATRIX q, p, qi;
	q = D3DXMATRIX(	qREAL, -qI,		-qJ,	-qK,
					qI,		qREAL,	-qK,	qJ,
					qJ,		qK,		qREAL,	-qI,
					qK,		-qJ,	qI,		qREAL		);

	p = D3DXMATRIX(	pREAL, -pI,		-pJ,	-pK,
					pI,		pREAL,	-pK,	pJ,
					pJ,		pK,		pREAL,	-pI,
					pK,		-pJ,	pI,		pREAL		);

	qi = D3DXMATRIX(qiREAL, 0,		0,		0,
					qiI,	0,		0,		0,
					qiJ,	0,		0,		0,
					qiK,	0,		0,		0		);

	D3DXMATRIX resultantMatrix;
	resultantMatrix = q * p * qi;
	D3DXVECTOR3 resultantPoint(resultantMatrix(1,0),resultantMatrix(2,0),resultantMatrix(3,0));
	cameraLookAt = resultantPoint + cameraPos;

	//Do it again for upVector: but this only works if upVector was defined correctly
	pREAL = 0;
	pI = upVector[0];
	pJ = upVector[1];
	pK = upVector[2];

	p = D3DXMATRIX(	pREAL, -pI,		-pJ,	-pK,
					pI,		pREAL,	-pK,	pJ,
					pJ,		pK,		pREAL,	-pI,
					pK,		-pJ,	pI,		pREAL		);

	resultantMatrix = q * p * qi;

	D3DXVECTOR3 resultantPoint2(resultantMatrix(1,0),resultantMatrix(2,0),resultantMatrix(3,0));
	upVector = resultantPoint2;
	makeItSo();
};

void camera::zoom(float value)
{
	//Camera Zoom: cameraLookAt remains in place while cameraPosition moves forward or backward
	//			in the direction determined by lookAtVector

	D3DXVECTOR3 lookAtVector = cameraPos - cameraLookAt;

	//NORMALIZE
	float length = sqrt( (lookAtVector[0]*lookAtVector[0])
						+(lookAtVector[1]*lookAtVector[1])
						+(lookAtVector[2]*lookAtVector[2]) );

	cameraPos[0] = cameraPos[0] + (lookAtVector[0] / length) * value;
	cameraPos[1] = cameraPos[1] + (lookAtVector[1] / length) * value;
	cameraPos[2] = cameraPos[2] + (lookAtVector[2] / length) * value;

	makeItSo();
};

void camera::moveIn(float value)
{
	//Camera MoveIn: cameraLookAt and cameraPosition move forward or backward
	//			in the direction determined by lookAtVector

	D3DXVECTOR3 lookAtVector = cameraPos - cameraLookAt;

	//NORMALIZE
	float length = sqrt( (lookAtVector[0]*lookAtVector[0])
						+(lookAtVector[1]*lookAtVector[1])
						+(lookAtVector[2]*lookAtVector[2]) );

	cameraPos[0] = cameraPos[0] + (lookAtVector[0] / length) * value;
	cameraPos[1] = cameraPos[1] + (lookAtVector[1] / length) * value;
	cameraPos[2] = cameraPos[2] + (lookAtVector[2] / length) * value;

	cameraLookAt[0] = cameraLookAt[0] + (lookAtVector[0] / length) * value;
	cameraLookAt[1] = cameraLookAt[1] + (lookAtVector[1] / length) * value;
	cameraLookAt[2] = cameraLookAt[2] + (lookAtVector[2] / length) * value;

	makeItSo();

};

void camera::pan(float horizontal, float vertical)
{	
	//Camera Pan: horizontal and vertical
	//Assumption: cameraPos AND cameraLookAt translate relative to upVector (vertical) and 
	//			the line perpendicular to the plane defined by the upVector and lookAtVector

	D3DXVECTOR3 lookAtVector = cameraPos - cameraLookAt;
	D3DXVECTOR3 horizontalVector, verticalVector;

	//Calculate Cross Product: lookAtVector X upVector
	horizontalVector[0] = lookAtVector[1] * upVector[2] - lookAtVector[2] * upVector[1];
    horizontalVector[1] = lookAtVector[2] * upVector[0] - lookAtVector[0] * upVector[2];
    horizontalVector[2] = lookAtVector[0] * upVector[1] - lookAtVector[1] * upVector[0];	
	//NORMALIZE!!
	float length = sqrt( (horizontalVector[0]*horizontalVector[0])
						+(horizontalVector[1]*horizontalVector[1])
						+(horizontalVector[2]*horizontalVector[2]) );
	horizontalVector[0] = horizontalVector[0] / length;
	horizontalVector[1] = horizontalVector[1] / length;
	horizontalVector[2] = horizontalVector[2] / length;

	verticalVector = upVector;
	//NORMALIZE!!
	length = sqrt( (verticalVector[0]*verticalVector[0])
						+(verticalVector[1]*verticalVector[1])
						+(verticalVector[2]*verticalVector[2]) );
	verticalVector[0] = verticalVector[0] / length;
	verticalVector[1] = verticalVector[1] / length;
	verticalVector[2] = verticalVector[2] / length;

	cameraPos	=	cameraPos + horizontal*horizontalVector + vertical*verticalVector;
	cameraLookAt	=	cameraLookAt + horizontal*horizontalVector + vertical*verticalVector;
	makeItSo();
};

void camera::makeItSo()
{
	D3DXMatrixLookAtLH(&View, &cameraPos, &cameraLookAt, &upVector);
	Device->SetTransform(D3DTS_VIEW, &View);
};

