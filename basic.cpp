#include "basic.h"
#include "globject.h"
#include <cmath>
#include "glut.h"


void Basic::setDefaultMaterial(){
	GLfloat ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat nothing[4] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, nothing);
	glMaterialfv(GL_FRONT, GL_EMISSION, nothing);
}


void Basic::addCube(float len){
	glObject obj;
	obj.pointSize = 8;
	obj.points = new sPoint[8];
	for (int i = 0; i < 8; ++i){
		obj.points[i].x = (i & 1) ? -0.5 * len : 0.5 * len;
		obj.points[i].y = (i & 2) ? -0.5 * len : 0.5 * len;
		obj.points[i].z = (i & 4) ? -0.5 * len : 0.5 * len;
	}
	obj.planeSize = 12;
	obj.planes = new sPlane[12];
	obj.planes[0].p[0] = 0; obj.planes[0].p[1] = 1; obj.planes[0].p[2] = 2;
	obj.planes[1].p[0] = 1; obj.planes[1].p[1] = 3; obj.planes[1].p[2] = 2;
	obj.planes[2].p[0] = 4; obj.planes[2].p[1] = 6; obj.planes[2].p[2] = 5;
	obj.planes[3].p[0] = 5; obj.planes[3].p[1] = 6; obj.planes[3].p[2] = 7;
	obj.planes[4].p[0] = 4; obj.planes[4].p[1] = 0; obj.planes[4].p[2] = 2;
	obj.planes[5].p[0] = 4; obj.planes[5].p[1] = 2; obj.planes[5].p[2] = 6;
	obj.planes[6].p[0] = 5; obj.planes[6].p[1] = 7; obj.planes[6].p[2] = 3;
	obj.planes[7].p[0] = 5; obj.planes[7].p[1] = 3; obj.planes[7].p[2] = 1;
	obj.planes[8].p[0] = 1; obj.planes[8].p[1] = 0; obj.planes[8].p[2] = 4;
	obj.planes[9].p[0] = 1; obj.planes[9].p[1] = 4; obj.planes[9].p[2] = 5;
	obj.planes[10].p[0] = 2; obj.planes[10].p[1] = 3; obj.planes[10].p[2] = 6;
	obj.planes[11].p[0] = 6; obj.planes[11].p[1] = 3; obj.planes[11].p[2] = 7;

	obj.planes[0].tex[0] = 0; obj.planes[0].tex[1] = 1; obj.planes[0].tex[2] = 2;
	obj.planes[1].tex[0] = 1; obj.planes[1].tex[1] = 3; obj.planes[1].tex[2] = 2;
	obj.planes[2].tex[0] = 0; obj.planes[2].tex[1] = 1; obj.planes[2].tex[2] = 2;
	obj.planes[3].tex[0] = 2; obj.planes[3].tex[1] = 1; obj.planes[3].tex[2] = 3;
	obj.planes[4].tex[0] = 0; obj.planes[4].tex[1] = 1; obj.planes[4].tex[2] = 3;
	obj.planes[5].tex[0] = 0; obj.planes[5].tex[1] = 3; obj.planes[5].tex[2] = 2;
	obj.planes[6].tex[0] = 0; obj.planes[6].tex[1] = 2; obj.planes[6].tex[2] = 3;
	obj.planes[7].tex[0] = 0; obj.planes[7].tex[1] = 3; obj.planes[7].tex[2] = 1;
	obj.planes[8].tex[0] = 1; obj.planes[8].tex[1] = 0; obj.planes[8].tex[2] = 2;
	obj.planes[9].tex[0] = 1; obj.planes[9].tex[1] = 2; obj.planes[9].tex[2] = 3;
	obj.planes[10].tex[0] = 0; obj.planes[10].tex[1] = 1; obj.planes[10].tex[2] = 2;
	obj.planes[11].tex[0] = 2; obj.planes[11].tex[1] = 1; obj.planes[11].tex[2] = 3;

	obj.texPoints = new Vec2f[4];
	obj.texPoints[0] = Vec2f(0, 0);
	obj.texPoints[1] = Vec2f(0, 1);
	obj.texPoints[2] = Vec2f(1, 0);
	obj.texPoints[3] = Vec2f(1, 1);

	for (int i = 0; i < obj.planeSize; ++i){
		Vec3f a(obj.points[obj.planes[i].p[1]].x - obj.points[obj.planes[i].p[0]].x,
			obj.points[obj.planes[i].p[1]].y - obj.points[obj.planes[i].p[0]].y,
			obj.points[obj.planes[i].p[1]].z - obj.points[obj.planes[i].p[0]].z);
		Vec3f b(obj.points[obj.planes[i].p[2]].x - obj.points[obj.planes[i].p[0]].x,
			obj.points[obj.planes[i].p[2]].y - obj.points[obj.planes[i].p[0]].y,
			obj.points[obj.planes[i].p[2]].z - obj.points[obj.planes[i].p[0]].z);
		Vec3f n = a * b;
		for (int j = 0; j < 3; ++j){
			obj.planes[i].normal[j].x = n[0];
			obj.planes[i].normal[j].y = n[1];
			obj.planes[i].normal[j].z = n[2];
		}
	}
	setConnectivity(obj);
	for (int i = 0; i < obj.planeSize; ++i)
		calPlane(obj, obj.planes[i]);
	objs.push_back(obj);
}
void Basic::loadTex(int i, char* file){
	TextureImage tex;
	LoadBMP(&tex, file);
	objs[i].tex.texture = tex;
}
void Basic::openTex(int i){
	if (objs[i].tex.texture.imageData == NULL) return;
	objs[i].hasTex = true;
}
void Basic::closeTex(int i){
	objs[i].hasTex = false;
}

void Basic::addSphere(float radius, int slices){
	int stack = slices;
	float alpha = atan2(0, -1) / stack;
	float ty = radius;
	float tr = 0;
	vector<sPoint> points;
	vector<sPlane> planes;
	glObject obj;
	float lx = 1.0 / slices;
	float ly = 1.0 / stack;
	obj.texPoints = new Vec2f[(slices + 1) * (stack + 1)];
	for (int i = 0; i <= stack; ++i){
		int k = stack - i;
		for (int j = 0; j <= slices; ++j){
			float px = lx * j;
			float py = ly * k;
			obj.texPoints[j + i * (stack + 1)] = Vec2f(px, py);
		}
	}
	for (int i = 1; i <= stack; ++i){
		float y = radius * cos(alpha * i);
		float r = radius * sin(alpha * i);
		float theta = 2.0 * atan2(0, -1) / slices;
		for (int j = 0; j < slices; ++j){
			int k = (j + 1) % slices;
			sPoint v1(tr * cos(theta * j), ty, tr * sin(theta * j));
			sPoint v2(tr * cos(theta * k), ty, tr * sin(theta * k));
			sPoint v3(r * cos(theta * j), y, r * sin(theta * j));
			sPoint v4(r * cos(theta * k), y, r * sin(theta * k));
			int beg = points.size();

			points.push_back(v1);
			if (i != 1)
				points.push_back(v2);
			points.push_back(v3);
			if (i != stack)
				points.push_back(v4);
			sPlane p1, p2;
			if (i != 1){
				p1.p[0] = beg + 0; p1.p[1] = beg + 1; p1.p[2] = beg + 2;
				p1.tex[0] = (i - 1) * (stack + 1) + j;
				p1.tex[1] = (i - 1) * (stack + 1) + j + 1;
				p1.tex[2] = i * (stack + 1) + j;
				p1.normal[0] = v1; p1.normal[1] = v2; p1.normal[2] = v3;
			}
			else{
				p1.p[0] = beg + 0; p1.p[1] = beg + 2; p1.p[2] = beg + 1;
				p1.tex[0] = (i - 1) * (stack + 1) + j;
				p1.tex[2] = (i - 1) * (stack + 1) + j + 1;
				p1.tex[1] = i * (stack + 1) + j;
				p1.normal[0] = v1; p1.normal[1] = v3; p1.normal[2] = v2;
			}

			p2.p[0] = beg + 1; p2.p[1] = beg + 3; p2.p[2] = beg + 2;
			p2.tex[0] = (i - 1) * (stack + 1) + j + 1;
			p2.tex[1] = i * (stack + 1) + j + 1;
			p2.tex[2] = i * (stack + 1) + j;
			p2.normal[0] = v2; p2.normal[1] = v4; p2.normal[2] = v3;
			planes.push_back(p1);
			if (i != 1 && i != stack)
				planes.push_back(p2);
		}
		tr = r;
		ty = y;
	}

	obj.pointSize = points.size();
	obj.planeSize = planes.size();
	obj.points = new sPoint[obj.pointSize];
	obj.planes = new sPlane[obj.planeSize];
	for (int i = 0; i < obj.pointSize; ++i)
		obj.points[i] = points[i];
	for (int i = 0; i < obj.planeSize; ++i)
		obj.planes[i] = planes[i];
	setConnectivity(obj);
	for (int i = 0; i < obj.planeSize; ++i)
		calPlane(obj, obj.planes[i]);
	objs.push_back(obj);
}

void Basic::addCylinder2(float baseR, float topR, float height, int slices){
	glObject obj;
	vector<sPoint> points;
	vector<sPlane> planes;
	float alpha = 2.0 * atan2(0, -1) / slices;
	obj.texPoints = new Vec2f[3 * (slices + 1)];
	float lx = 1.0 / slices;
	for (int i = 0; i <= slices; ++i){
		float x = lx * i;
		obj.texPoints[i] = Vec2f(x, 1);
	}
	for (int i = 0; i <= slices; ++i){
		float x = lx * i;
		obj.texPoints[i + slices + 1] = Vec2f(x, 0);
	}
	for (int i = 0; i < slices; ++i){
		float x = 0.5 * cos(alpha * i) + 0.5;
		float y = 0.5 * sin(alpha * i) + 0.5;
		obj.texPoints[i + 2 * slices + 2] = Vec2f(x, y);
	}
	obj.texPoints[3 * slices + 2] = Vec2f(0.5, 0.5);
	sPoint baseC(0, 0, 0);
	points.push_back(baseC);
	for (int i = 0; i < slices; ++i){
		float x = baseR * cos(alpha * i);
		float z = baseR * sin(alpha * i);
		points.push_back(sPoint(x, 0, z));
	}
	for (int i = 0; i < slices; ++i){
		sPlane p;
		p.p[0] = 0; p.p[1] = i + 1; p.p[2] = (i + 1) % slices + 1;
		for (int j = 0; j < 3; ++j)
			p.normal[j] = sPoint(0, -1, 0);
		p.tex[0] = 3 * slices + 2; p.tex[1] = 2 * slices + 2 + i; p.tex[2] = 2 * slices + 2 + (i + 1) % slices;
		planes.push_back(p);
	}
	sPoint topC(0, height, 0);
	points.push_back(topC);
	for (int i = 0; i < slices; ++i){
		float x = topR * cos(alpha * i);
		float z = topR * sin(alpha * i);
		points.push_back(sPoint(x, height, z));
	}
	for (int i = 0; i < slices; ++i){
		sPlane p;
		p.p[0] = slices + 1; p.p[2] = slices + i + 2; p.p[1] = slices + (i + 1) % slices + 2;
		for (int j = 0; j < 3; ++j)
			p.normal[j] = sPoint(0, 1, 0);
		p.tex[0] = 3 * slices + 2; p.tex[2] = 2 * slices + 2 + i; p.tex[1] = 2 * slices + 2 + (i + 1) % slices;
		planes.push_back(p);
	}
	for (int i = 0; i < slices; ++i){
		int j = (i + 1) % slices;
		int v1 = i + 1;
		int v2 = j + 1;
		int v3 = slices + i + 2;
		int v4 = slices + j + 2;
		float bxi = baseR * cos(alpha * i);
		float bzi = baseR * sin(alpha * i);
		float txi = topR * cos(alpha * i);
		float tzi = topR * sin(alpha * i);
		float bxj = baseR * cos(alpha * j);
		float bzj = baseR * sin(alpha * j);
		float txj = topR * cos(alpha * j);
		float tzj = topR * sin(alpha * j);
		sPoint n1(bxi, (baseR - topR) * baseR / height, bzi);
		sPoint n2(bxj, (baseR - topR) * baseR / height, bzj);
		sPoint n3(txi, (baseR - topR) * topR / height, tzi);
		sPoint n4(txj, (baseR - topR) * topR / height, tzj);
		sPlane p1, p2;
		p1.p[0] = v1; p1.p[1] = v3; p1.p[2] = v2;
		p1.tex[0] = slices + 1 + i; p1.tex[1] = i; p1.tex[2] = slices + 2 + i;
		p2.p[0] = v2; p2.p[1] = v3; p2.p[2] = v4;
		p2.tex[0] = slices + 2 + i; p2.tex[1] = i; p2.tex[2] = i + 1;
		p1.normal[0] = n1; p1.normal[1] = n3; p1.normal[2] = n2;
		p2.normal[0] = n2; p2.normal[1] = n3; p2.normal[2] = n4;
		planes.push_back(p1);
		planes.push_back(p2);
	}
	obj.pointSize = points.size();
	obj.planeSize = planes.size();
	obj.points = new sPoint[obj.pointSize];
	obj.planes = new sPlane[obj.planeSize];
	for (int i = 0; i < obj.pointSize; ++i)
		obj.points[i] = points[i];
	for (int i = 0; i < obj.planeSize; ++i)
		obj.planes[i] = planes[i];
	setConnectivity(obj);
	for (int i = 0; i < obj.planeSize; ++i)
		calPlane(obj, obj.planes[i]);
	objs.push_back(obj);
}

void Basic::addCylinder(float radius, float height, int slice){
	addCylinder2(radius, radius, height, slice);
}

void Basic::addCone(float radius, float height, int slices){
	glObject obj;
	vector<sPoint> points;
	vector<sPlane> planes;
	float alpha = 2.0 * atan2(0, -1) / slices;
	obj.texPoints = new Vec2f[slices + 1];
	for (int i = 0; i < slices; ++i){
	float x = 0.5 * cos(alpha * i) + 0.5;
	float y = 0.5 * sin(alpha * i) + 0.5;
	obj.texPoints[i] = Vec2f(x, y);
	}
	obj.texPoints[slices] = Vec2f(0.5, 0.5);
	sPoint baseC(0, 0, 0);
	points.push_back(baseC);
	for (int i = 0; i < slices; ++i){
		float x = radius * cos(alpha * i);
		float z = radius * sin(alpha * i);
		points.push_back(sPoint(x, 0, z));
	}
	for (int i = 0; i < slices; ++i){
		sPlane p;
		p.p[0] = 0; p.p[1] = i + 1; p.p[2] = (i + 1) % slices + 1;
		p.tex[0] = slices; p.tex[1] = i; p.tex[2] = (i + 1) % slices;
		for (int j = 0; j < 3; ++j)
			p.normal[j] = sPoint(0, -1, 0);
		planes.push_back(p);
	}
	sPoint topC(0, height, 0);
	points.push_back(topC);
	for (int i = 0; i < slices; ++i){
		int j = (i + 1) % slices;
		int v1 = slices + 1;
		int v2 = i + 1;
		int v3 = j + 1;
		float xi = radius * cos(alpha * i);
		float zi = radius * sin(alpha * i);
		float xj = radius * cos(alpha * j);
		float zj = radius * sin(alpha * j);
		float cx = radius * cos(alpha * (i + i + 1) / 2);
		float cz = radius * sin(alpha * (i + i + 1) / 2);
		sPoint n1(cx, radius * radius / height, cz);
		sPoint n2(xi, radius * radius / height, zi);
		sPoint n3(xj, radius * radius / height, zj);
		sPlane p1;
		p1.p[0] = v1; p1.p[1] = v3; p1.p[2] = v2;
		p1.normal[0] = n1; p1.normal[1] = n3; p1.normal[2] = n2;
		p1.tex[0] = slices; p1.tex[2] = i; p1.tex[1] = (i + 1) % slices;
		planes.push_back(p1);
	}
	obj.pointSize = points.size();
	obj.planeSize = planes.size();
	obj.points = new sPoint[obj.pointSize];
	obj.planes = new sPlane[obj.planeSize];
	for (int i = 0; i < obj.pointSize; ++i)
		obj.points[i] = points[i];
	for (int i = 0; i < obj.planeSize; ++i)
		obj.planes[i] = planes[i];
	setConnectivity(obj);
	for (int i = 0; i < obj.planeSize; ++i)
		calPlane(obj, obj.planes[i]);
	objs.push_back(obj);
}

void Basic::addCone2(float radius, float height, int slices){
	glObject obj;
	vector<sPoint> points;
	vector<sPlane> planes;
	float alpha = 2.0 * atan2(0, -1) / slices;
	obj.texPoints = new Vec2f[slices + 1];
	for (int i = 0; i < slices; ++i){
	float x = 0.5 * cos(alpha * i) + 0.5;
	float y = 0.5 * sin(alpha * i) + 0.5;
	obj.texPoints[i] = Vec2f(x, y);
	}
	obj.texPoints[slices] = Vec2f(0.5, 0.5);
	sPoint baseC(0, 0, 0);
	points.push_back(baseC);
	for (int i = 0; i < slices; ++i){
		float x = radius * cos(alpha * i);
		float z = radius * sin(alpha * i);
		points.push_back(sPoint(x, 0, z));
	}
	for (int i = 0; i < slices; ++i){
		sPlane p;
		p.p[0] = 0; p.p[1] = i + 1; p.p[2] = (i + 1) % slices + 1;
		p.tex[0] = slices; p.tex[1] = i; p.tex[2] = (i + 1) % slices;
		for (int j = 0; j < 3; ++j)
			p.normal[j] = sPoint(0, -1, 0);
		planes.push_back(p);
	}
	sPoint topC(0, height, 0);
	points.push_back(topC);
	for (int i = 0; i < slices; ++i){
		int j = (i + 1) % slices;
		int v1 = slices + 1;
		int v2 = i + 1;
		int v3 = j + 1;
		Vec3f a(points[v3].x - points[v1].x,
			points[v3].y - points[v1].y,
			points[v3].z - points[v1].z);
		Vec3f b(points[v2].x - points[v1].x,
			points[v2].y - points[v1].y,
			points[v2].z - points[v1].z);
		Vec3f n = a * b;
		sPoint norm(n[0], n[1], n[2]);
		sPlane p1;
		p1.p[0] = v1; p1.p[1] = v3; p1.p[2] = v2;
		p1.tex[0] = slices; p1.tex[2] = i; p1.tex[1] = (i + 1) % slices;
		p1.normal[0] = norm; p1.normal[1] = norm; p1.normal[2] = norm;
		planes.push_back(p1);
	}
	obj.pointSize = points.size();
	obj.planeSize = planes.size();
	obj.points = new sPoint[obj.pointSize];
	obj.planes = new sPlane[obj.planeSize];
	for (int i = 0; i < obj.pointSize; ++i)
		obj.points[i] = points[i];
	for (int i = 0; i < obj.planeSize; ++i)
		obj.planes[i] = planes[i];
	setConnectivity(obj);
	for (int i = 0; i < obj.planeSize; ++i)
		calPlane(obj, obj.planes[i]);
	objs.push_back(obj);
}

void Basic::addPrism2(float baseR, float topR, float height, int slices){
	glObject obj;
	vector<sPoint> points;
	vector<sPlane> planes;
	float alpha = 2.0 * atan2(0, -1) / slices;
	obj.texPoints = new Vec2f[slices + 5];
	for (int i = 0; i < slices; ++i){
		float x = 0.5 * cos(alpha * i) + 0.5;
		float y = 0.5 * sin(alpha * i) + 0.5;
		obj.texPoints[i] = Vec2f(x, y);
	}
	obj.texPoints[slices] = Vec2f(0.5, 0.5);
	obj.texPoints[slices + 1] = Vec2f(0, 0);
	obj.texPoints[slices + 2] = Vec2f(0, 1);
	obj.texPoints[slices + 3] = Vec2f(1, 0);
	obj.texPoints[slices + 4] = Vec2f(1, 1);
	sPoint baseC(0, 0, 0);
	points.push_back(baseC);
	for (int i = 0; i < slices; ++i){
		float x = baseR * cos(alpha * i);
		float z = baseR * sin(alpha * i);
		points.push_back(sPoint(x, 0, z));
	}
	for (int i = 0; i < slices; ++i){
		sPlane p;
		p.p[0] = 0; p.p[1] = i + 1; p.p[2] = (i + 1) % slices + 1;
		p.tex[0] = slices; p.tex[1] = i; p.tex[2] = (i + 1) % slices;
		for (int j = 0; j < 3; ++j)
			p.normal[j] = sPoint(0, -1, 0);
		planes.push_back(p);
	}
	sPoint topC(0, height, 0);
	points.push_back(topC);
	for (int i = 0; i < slices; ++i){
		float x = topR * cos(alpha * i);
		float z = topR * sin(alpha * i);
		points.push_back(sPoint(x, height, z));
	}
	for (int i = 0; i < slices; ++i){
		sPlane p;
		p.p[0] = slices + 1; p.p[2] = slices + i + 2; p.p[1] = slices + (i + 1) % slices + 2;
		p.tex[0] = slices; p.tex[2] = i; p.tex[1] = (i + 1) % slices;
		for (int j = 0; j < 3; ++j)
			p.normal[j] = sPoint(0, 1, 0);
		planes.push_back(p);
	}
	for (int i = 0; i < slices; ++i){
		int j = (i + 1) % slices;
		int v1 = i + 1;
		int v2 = j + 1;
		int v3 = slices + i + 2;
		int v4 = slices + j + 2;
		Vec3f a(points[v3].x - points[v2].x,
			points[v3].y - points[v2].y,
			points[v3].z - points[v2].z);
		Vec3f b(points[v4].x - points[v1].x,
			points[v4].y - points[v1].y,
			points[v4].z - points[v1].z);
		Vec3f n = a * b;
		sPoint norm(n[0], n[1], n[2]);
		sPlane p1, p2;
		p1.p[0] = v1; p1.p[1] = v3; p1.p[2] = v2;
		p1.tex[0] = slices + 1; p1.tex[1] = slices + 2; p1.tex[2] = slices + 3;
		p2.p[0] = v2; p2.p[1] = v3; p2.p[2] = v4;
		p2.tex[0] = slices + 3; p2.tex[1] = slices + 2; p2.tex[2] = slices + 4;
		p1.normal[0] = norm; p1.normal[1] = norm; p1.normal[2] = norm;
		p2.normal[0] = norm; p2.normal[1] = norm; p2.normal[2] = norm;
		planes.push_back(p1);
		planes.push_back(p2);
	}
	obj.pointSize = points.size();
	obj.planeSize = planes.size();
	obj.points = new sPoint[obj.pointSize];
	obj.planes = new sPlane[obj.planeSize];
	for (int i = 0; i < obj.pointSize; ++i)
		obj.points[i] = points[i];
	for (int i = 0; i < obj.planeSize; ++i)
		obj.planes[i] = planes[i];
	setConnectivity(obj);
	for (int i = 0; i < obj.planeSize; ++i)
		calPlane(obj, obj.planes[i]);
	objs.push_back(obj);
}

void Basic::addPrism(float radius, float height, int slices){
	addPrism2(radius, radius, height, slices);
}

void Basic::addCircle(float radius, int slices){
	glObject obj;
	vector<sPoint> points;
	vector<sPlane> planes;
	float alpha = 2.0 * atan2(0, -1) / slices;
	obj.texPoints = new Vec2f[slices + 1];
	for (int i = 0; i < slices; ++i){
		float x = 0.5 * cos(alpha * i) + 0.5;
		float y = 0.5 * sin(alpha * i) + 0.5;
		obj.texPoints[i] = Vec2f(x, y);
	}
	obj.texPoints[slices] = Vec2f(0.5, 0.5);
	sPoint baseC(0, 0, 0);
	points.push_back(baseC);
	for (int i = 0; i < slices; ++i){
		float x = radius * cos(alpha * i);
		float z = radius * sin(alpha * i);
		points.push_back(sPoint(x, 0, z));
	}
	for (int i = 0; i < slices; ++i){
		sPlane p1, p2;
		p1.p[0] = 0; p1.p[2] = i + 1; p1.p[1] = (i + 1) % slices + 1;
		p1.tex[0] = slices; p1.tex[2] = i; p1.tex[1] = (i + 1) % slices + 1;
		p2.p[0] = 0; p2.p[1] = i + 1; p2.p[2] = (i + 1) % slices + 1;
		p1.tex[0] = slices; p1.tex[1] = i; p1.tex[2] = (i + 1) % slices + 1;
		for (int j = 0; j < 3; ++j){
			p1.normal[j] = sPoint(0, 1, 0);
			p2.normal[j] = sPoint(0, -1, 0);
		}
		planes.push_back(p1);
		planes.push_back(p2);
	}
	obj.pointSize = points.size();
	obj.planeSize = planes.size();
	obj.points = new sPoint[obj.pointSize];
	obj.planes = new sPlane[obj.planeSize];
	for (int i = 0; i < obj.pointSize; ++i)
		obj.points[i] = points[i];
	for (int i = 0; i < obj.planeSize; ++i)
		obj.planes[i] = planes[i];
	setConnectivity(obj);
	for (int i = 0; i < obj.planeSize; ++i)
		calPlane(obj, obj.planes[i]);
	objs.push_back(obj);
}

void Basic::addPlane(float len, int slices){
	glObject obj;
	obj.texPoints = new Vec2f[4];
	obj.texPoints[0] = Vec2f(0, 0);
	obj.texPoints[1] = Vec2f(1, 0);
	obj.texPoints[2] = Vec2f(0, 1);
	obj.texPoints[3] = Vec2f(1, 1);
	obj.pointSize = 4;
	obj.planeSize = 4;
	obj.points = new sPoint[obj.pointSize];
	obj.planes = new sPlane[obj.planeSize];
	for (int i = 0; i < 4; ++i){
		obj.points[i].x = (i & 1) ? 0.5 * len : -0.5 * len;
		obj.points[i].y = 0;
		obj.points[i].z = (i & 2) ? 0.5 * len : -0.5 * len;
	}
	sPlane p1, p2;
	sPoint norm(0, 1, 0);
	p1.p[0] = 0; p1.p[1] = 2; p1.p[2] = 1;
	p1.tex[0] = 1; p1.tex[1] = 3; p1.tex[2] = 0;
	p2.p[0] = 1; p2.p[1] = 2; p2.p[2] = 3;
	p2.tex[0] = 0; p2.tex[1] = 3; p2.tex[2] = 2;
	for (int i = 0; i < 3; ++i){
		p1.normal[i] = norm;
		p2.normal[i] = norm;
	}
	obj.planes[0] = p1;
	obj.planes[1] = p2;
	norm = sPoint(0, -1, 0);
	p1.p[0] = 0; p1.p[1] = 1; p1.p[2] = 2;
	p1.tex[0] = 1; p1.tex[1] = 0; p1.tex[2] = 3;
	p2.p[0] = 1; p2.p[1] = 3; p2.p[2] = 2;
	p2.tex[0] = 0; p2.tex[1] = 2; p2.tex[2] = 3;
	for (int i = 0; i < 3; ++i){
		p1.normal[i] = norm;
		p2.normal[i] = norm;
	}
	obj.planes[2] = p1;
	obj.planes[3] = p2;

	objs.push_back(obj);
}

void Basic::drawAll(){
	for (int i = 0; i < objs.size(); ++i){
		glPushMatrix();
		glTranslatef(objs[i].t[0], objs[i].t[1], objs[i].t[2]);
		glScalef(objs[i].s[0], objs[i].s[1], objs[i].s[2]);
		glRotatef(objs[i].r[2], 0, 0, 1);
		glRotatef(objs[i].r[1], 0, 1, 0);
		glRotatef(objs[i].r[0], 1, 0, 0);
		drawGLObject(objs[i]);
		glPopMatrix();
	}
}












































void Basic::drawCube(Vec3f center, Vec3f size){
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glScalef(size[0], size[1], size[2]);
	glutSolidCube(1.0);
	glPopMatrix();
}
void Basic::drawSphere(Vec3f center, Vec3f size){
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glScalef(size[0], size[1], size[2]);
	glutSolidSphere(1.0, 16, 16);
	glPopMatrix();
}

void Basic::drawCylinder(Vec2f size, Vec3f center, int slices){
	drawCylinder2(size[0], size[0], size[1], center, slices);
}
void Basic::drawCylinder2(float baseR, float topR, float height, Vec3f center, int slices){
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(gluNewQuadric(), baseR, topR, height, slices, 16);
	glPopMatrix();
	drawCircle(baseR, 16, -1);
	glPushMatrix();
	glTranslatef(0, height, 0);
	drawCircle(topR, 16, 1);
	glPopMatrix();
	glPopMatrix();
}

void Basic::drawCone(float radius, float height, int slices, Vec3f center){
	drawCylinder2(radius, 0, height, center, slices);
}
void Basic::drawCone2(float radius, float height, int slices, Vec3f center){
	drawPrism2(radius, 0, height, slices, center);
}

void Basic::drawPrism(float radius, float height, int slices, Vec3f baseCenter){
	drawPrism2(radius, radius, height, slices, baseCenter);
}
void Basic::drawPrism2(float baseR, float topR, float height, int slices, Vec3f baseCenter){
	glPushMatrix();
	glTranslatef(baseCenter[0], baseCenter[1], baseCenter[2]);
	glPushMatrix();
	//gluCylinder(gluNewQuadric(), baseR, topR, height, slices, 16);
	drawPrismSide(baseR, topR, height, slices);
	glPopMatrix();
	drawCircle(baseR, slices, -1);
	glPushMatrix();
	glTranslatef(0, height, 0);
	drawCircle(topR, slices, 1);
	glPopMatrix();
	glPopMatrix();
}
void Basic::drawPrismSide(float baseR, float topR, float height, int slices){
	glPushMatrix();

	GLfloat angle = 2 * atan2(0, -1) / slices;
	GLfloat vertexBase[2][3] = { { 0, 0, 0 }, { 0, 0, 0 } };
	GLfloat vertexTop[2][3] = { { 0, height, 0 }, { 0, height, 0 } };
	bool s = 0;
	glBegin(GL_QUADS);
	vertexBase[s][0] = baseR * 1.0;
	vertexBase[s][2] = baseR * 0.0;
	vertexTop[s][0] = topR * 1.0;
	vertexTop[s][2] = topR * 0.0;


	for (int i = 1; i < slices; ++i){
		vertexBase[!s][0] = baseR * cos(angle * i);
		vertexBase[!s][2] = baseR * sin(angle * i);
		vertexTop[!s][0] = topR * cos(angle * i);
		vertexTop[!s][2] = topR * sin(angle * i);
		Vec3f a(vertexTop[s][0] - vertexBase[s][0], vertexTop[s][1] - vertexBase[s][1],
			vertexTop[s][2] - vertexBase[s][2]);
		Vec3f b(vertexBase[!s][0] - vertexBase[s][0], vertexBase[!s][1] - vertexBase[s][1],
			vertexBase[!s][2] - vertexBase[s][2]);
		Vec3f n = a * b;
		glNormal3f(n[0], n[1], n[2]);
		glVertex3fv(vertexBase[s]);
		glVertex3fv(vertexTop[s]);
		glVertex3fv(vertexTop[!s]);
		glVertex3fv(vertexBase[!s]);

		s = !s;
	}
	vertexBase[!s][0] = baseR * 1.0;
	vertexBase[!s][2] = baseR * 0.0;
	vertexTop[!s][0] = topR * 1.0;
	vertexTop[!s][2] = topR * 0.0;
	Vec3f a(vertexTop[s][0] - vertexBase[s][0], vertexTop[s][1] - vertexBase[s][1],
		vertexTop[s][2] - vertexBase[s][2]);
	Vec3f b(vertexBase[!s][0] - vertexBase[s][0], vertexBase[!s][1] - vertexBase[s][1],
		vertexBase[!s][2] - vertexBase[s][2]);
	Vec3f n = a * b;
	glNormal3f(n[0], n[1], n[2]);
	glVertex3fv(vertexBase[s]);
	glVertex3fv(vertexTop[s]);
	glVertex3fv(vertexTop[!s]);
	glVertex3fv(vertexBase[!s]);
	glEnd();
	glPopMatrix();
}

void Basic::drawPlane(float length, int dir, Vec3f center){
	drawCircle(length / sqrt(2), 4, dir, center);
}
void Basic::drawCircle(float radius, int slices, int dir, Vec3f center){
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);

	GLfloat angle = 2 * atan2(0, -1) / slices;
	GLfloat vertex[3] = { 0, 0, 0 };
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, dir, 0);
	glVertex3fv(vertex);
	for (int i = 0; i < slices; ++i){
		vertex[0] = radius * cos(angle * i);
		vertex[2] = radius * sin(angle * i);
		glVertex3fv(vertex);
	}
	vertex[0] = 1.0 * radius;
	vertex[2] = 0.0 * radius;
	glVertex3fv(vertex);
	glEnd();
	glPopMatrix();
}
