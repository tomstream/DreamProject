#ifndef __VEC_H__
#define __VEC_H__
#include <cmath>
struct Vec3f{
	float x[3];
	Vec3f(){}
	Vec3f(float f1, float f2, float f3){
		x[0] = f1; x[1] = f2; x[2] = f3;
	}
	float& operator[](int i){
		return x[i];
	}
	friend Vec3f operator - (Vec3f &a, Vec3f &b){
		return Vec3f(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
	}
	friend Vec3f operator + (Vec3f &a, Vec3f &b){
		return Vec3f(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
	}
	friend Vec3f operator / (Vec3f &a, float &f){
		return Vec3f(a.x[0] / f, a.x[1] / f, a.x[2] / f);
	}
	friend Vec3f operator * (Vec3f &a, float &f){
		return Vec3f(a.x[0] * f, a.x[1] * f, a.x[2] * f);
	}
	friend Vec3f operator * (Vec3f &a, Vec3f &b){
		return Vec3f(a[1] * b[2] - a[2] * b[1],
			a[2] * b[0] - a[0] * b[2],
			a[0] * b[1] - a[1] * b[0]);
	}
	void setX(float f) { x[0] = f; }
	void setY(float f) { x[1] = f; }
	void setZ(float f) { x[2] = f; }
	void normalize(){
		float sum = 0;
		sum += sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
		x[0] /= sum;
		x[1] /= sum;
		x[2] /= sum;
	}
};
struct Vec2f{
	float x[2];
	Vec2f(){}
	Vec2f(float f1, float f2){
		x[0] = f1; x[1] = f2;
	}
	float& operator[](int i){
		return x[i];
	}
};

#endif