#include "globjmgr.h"
#include "globject.h"
#include "file.h"
#include <cmath>
#include "glut.h"

GLObjectManager::GLObjectManager(){
	vector<string> fileName;
	getFiles("texture", fileName);
	for (auto &str : fileName){
		string last3 = string(str.end() - 4, str.end());
		if (last3 != ".bmp"){
			continue;
		}
		else{
			texNameVec.push_back(str);
		}
	}
	fileName.clear();
	getFiles("obj", fileName);
	for (auto &str : fileName){
		string last3 = string(str.end() - 4, str.end());
		if (last3 != ".obj"){
			continue;
		}
		else{
			objNameVec.push_back(string(str.begin(), str.end() - 4));
		}
	}
}
void GLObjectManager::newId(){
	currentId++;
}

bool GLObjectManager::testCrash(Vec3f s, Vec3f d){
	for (int i = 0; i < objs.size(); ++i){
		glObject &o = objs[i];
		for (int j = 0; j < o.planeSize; ++j){
			GLmatrix16f Minv;
			GLvector4f start = { s[0], s[1], s[2], 1 };
			GLvector4f end = { s[0] + d[0], s[1] + d[1], s[2] + d[2], 1 };
			GLvector4f eq = { o.planes[j].PlaneEq.a, o.planes[j].PlaneEq.b,
				o.planes[j].PlaneEq.c, o.planes[j].PlaneEq.d };
			glLoadIdentity();
			glScalef(1.0 / o.s[0], 1.0 / o.s[1], 1.0 / o.s[2]);
			glRotatef(-o.r[0], 1, 0, 0);
			glRotatef(-o.r[1], 0, 1, 0);
			glRotatef(-o.r[2], 0, 0, 1);
			glTranslatef(-o.t[0], -o.t[1], -o.t[2]);
			glGetFloatv(GL_MODELVIEW_MATRIX, Minv);
			GLvector4f neq;
			for (int ki = 0; ki < 4; ++ki){
				neq[ki] = 0;
				for (int kj = 0; kj < 4; ++kj){
					neq[ki] += eq[kj] * Minv[ki * 4 + kj];
				}
			}
			glLoadIdentity();
			glTranslatef(o.t[0], o.t[1], o.t[2]);

			glRotatef(o.r[2], 0, 0, 1);
			glRotatef(o.r[1], 0, 1, 0);
			glRotatef(o.r[0], 1, 0, 0);
			glScalef(o.s[0], o.s[1], o.s[2]);
			glGetFloatv(GL_MODELVIEW_MATRIX, Minv);
			//VMatMult(Minv, start);
			//VMatMult(Minv, dir);

			/*float t = o.planes[j].normal[0].x * n[0] +
			o.planes[j].normal[0].y * n[1] +
			o.planes[j].normal[0].z * n[2];*/
			//if (t < 1e-5) continue;
			float side1 = neq[0] * start[0] + neq[1] * start[1] + neq[2] * start[2] + neq[3] * start[3];
			float side2 = neq[0] * end[0] + neq[1] * end[1] + neq[2] * end[2] + neq[3] * end[3];
			if (side1 >= -1e-3 && side2 >= -1e-3 || side1 <= 1e-3 && side2 <= 1e-3) continue;
			float t1 = (neq[0] * s[0] + neq[1] * s[1] + neq[2] * s[2]);
			float t2 = (neq[0] * d[0] + neq[1] * d[1] + neq[2] * d[2]);
			float t = (-neq[3] - t1) / t2;
			if (t < 1e-5) continue;
			GLvector4f x = { s[0] + d[0] * t, s[1] + d[1] * t, s[2] + d[2] * t, 1 };
			GLvector4f v0 = { o.points[o.planes[j].p[0]].x, o.points[o.planes[j].p[0]].y, o.points[o.planes[j].p[0]].z, 1 };
			GLvector4f v1 = { o.points[o.planes[j].p[1]].x, o.points[o.planes[j].p[1]].y, o.points[o.planes[j].p[1]].z, 1 };
			GLvector4f v2 = { o.points[o.planes[j].p[2]].x, o.points[o.planes[j].p[2]].y, o.points[o.planes[j].p[2]].z, 1 };

			VMatMult(Minv, v0);
			VMatMult(Minv, v1);
			VMatMult(Minv, v2);
			float ia = 0, ib = 0, ic = 0, ix = 0;
			for (int mi = 0; mi < 4; ++mi){
				ia += v0[mi] * neq[mi];
				ib += v1[mi] * neq[mi];
				ic += v2[mi] * neq[mi];
				ix += x[mi] * neq[mi];
			}
			Vec3f a(v0[0] - x[0], v0[1] - x[1], v0[2] - x[2]);
			Vec3f b(v1[0] - x[0], v1[1] - x[1], v1[2] - x[2]);
			Vec3f c(v2[0] - x[0], v2[1] - x[1], v2[2] - x[2]);
			Vec3f n1 = a * b;
			Vec3f n2 = b * c;
			Vec3f n3 = c * a;
			float a1 = dot(n1, n2);
			float a2 = dot(n2, n3);
			float a3 = dot(n1, n3);
			if (a1 >= 0 && a2 >= 0 && a3 >= 0){
				cout << "crash " << i << endl;
				return true;
			}
		}
	}
	return false;
}

void GLObjectManager::removeById(int id){
	if (ids.find(id) == ids.end()) return;
	while (ids.find(id) != ids.end())
		removeByIndex(ids[id]);
}

void GLObjectManager::removeByIndex(int index){
	objs.erase(objs.begin() + index);
	ids.clear();
	for (int i = objs.size() - 1; i >= 0; --i){
		ids[objs[i].id] = i;
	}
}

glObject& GLObjectManager::getById(int id){
	int index = ids[id];
	return getByIndex(index);
}

glObject& GLObjectManager::getByIndex(int index){
	return objs[index];
}

void GLObjectManager::setTexById(int id, int texId){
	if (ids.find(id) == ids.end()) return;
	for (int i = 0; i < objs.size(); ++i)
		if (objs[i].id == id)
			setTexByIndex(i, texId);
}
void GLObjectManager::loadTexById(int id, const char* file){
	if (ids.find(id) == ids.end()) return;
	for (int i = 0; i < objs.size(); ++i)
		if (objs[i].id == id)
			loadTexByIndex(i, file);
}
void GLObjectManager::openTexById(int id){
	if (ids.find(id) == ids.end()) return;
	for (int i = 0; i < objs.size(); ++i)
		if (objs[i].id == id)
			objs[i].hasTex = true;
}
void GLObjectManager::closeTexById(int id){
	if (ids.find(id) == ids.end()) return;
	for (int i = 0; i < objs.size(); ++i)
		if (objs[i].id == id)
			objs[i].hasTex = false;
}


void GLObjectManager::setTexByIndex(int index, int texId){
	objs[index].tex.texture.texID = texId;
}
void GLObjectManager::loadTexByIndex(int index, const char* file){
	TextureImage tex;
	LoadBMP(&tex, file);
	objs[index].tex.texture = tex;
}
void GLObjectManager::openTexByIndex(int index){
	objs[index].hasTex = true;
}
void GLObjectManager::closeTexByIndex(int index){
	objs[index].hasTex = false;
}
void GLObjectManager::setDefaultMaterial(){
	GLfloat ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat nothing[4] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, nothing);
	glMaterialfv(GL_FRONT, GL_EMISSION, nothing);
}

glObject& GLObjectManager::addObj(char *file){
	Obj o;
	o.read(file, currentId);
	ids[currentId] = objs.size();
	for (int i = 0; i < o.objs.size(); ++i){
		glObject obj = o.objs[i];
		obj.id = currentId;
		objs.push_back(obj);
	}
	objFile.insert(currentId);
	newId();
	return objs[ids[currentId - 1]];
}

void GLObjectManager::scale(int id, float *s){
	for (int i = 0; i < objs.size(); ++i){
		if (objs[i].id == id)
			for (int j = 0; j < 3; ++j){
			objs[i].s[j] *= s[j];
			}
	}
}

void GLObjectManager::translate(int id, float *t){
	for (int i = 0; i < objs.size(); ++i){
		if (objs[i].id == id)
			for (int j = 0; j < 3; ++j){
			objs[i].t[j] += t[j];
			}
	}
}

void GLObjectManager::rotate(int id, float *r){
	for (int i = 0; i < objs.size(); ++i){
		if (objs[i].id == id)
			for (int j = 0; j < 3; ++j){
			objs[i].r[j] += r[j];
			}
	}
}

glObject& GLObjectManager::addCube(float len){
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
	obj.id = currentId;
	ids[currentId] = objs.size();
	newId();
	objs.push_back(obj);
	return objs[objs.size() - 1];
}

glObject& GLObjectManager::addSphere(float radius, int slices){
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
			//if (i != 1)
			points.push_back(v2);
			points.push_back(v3);
			if (i != stack)
				points.push_back(v4);
			sPlane p1, p2;
			//if (i != 1){
			p1.p[0] = beg + 0; p1.p[1] = beg + 1; p1.p[2] = beg + 2;
			p1.tex[0] = (i - 1) * (stack + 1) + j;
			p1.tex[1] = (i - 1) * (stack + 1) + j + 1;
			p1.tex[2] = i * (stack + 1) + j;
			p1.normal[0] = v1; p1.normal[1] = v2; p1.normal[2] = v3;
			/*}
			else{
			p1.p[0] = beg + 0; p1.p[1] = beg + 2; p1.p[2] = beg + 1;
			p1.tex[0] = (i - 1) * (stack + 1) + j;
			p1.tex[2] = (i - 1) * (stack + 1) + j + 1;
			p1.tex[1] = i * (stack + 1) + j;
			p1.normal[0] = v1; p1.normal[1] = v3; p1.normal[2] = v2;
			}*/

			p2.p[0] = beg + 1; p2.p[1] = beg + 3; p2.p[2] = beg + 2;
			p2.tex[0] = (i - 1) * (stack + 1) + j + 1;
			p2.tex[1] = i * (stack + 1) + j + 1;
			p2.tex[2] = i * (stack + 1) + j;
			p2.normal[0] = v2; p2.normal[1] = v4; p2.normal[2] = v3;
			planes.push_back(p1);
			if (/*i != 1 &&*/ i != stack)
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
	obj.id = currentId;
	ids[currentId] = objs.size();
	newId();
	objs.push_back(obj);
	return objs[objs.size() - 1];
}

glObject& GLObjectManager::addCylinder2(float baseR, float topR, float height, int slices){
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
	obj.id = currentId;
	ids[currentId] = objs.size();
	newId();
	objs.push_back(obj);
	return objs[objs.size() - 1];
}

glObject& GLObjectManager::addCylinder(float radius, float height, int slice){
	return addCylinder2(radius, radius, height, slice);
}

glObject& GLObjectManager::addCone(float radius, float height, int slices){
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
	obj.id = currentId;
	ids[currentId] = objs.size();
	newId();
	objs.push_back(obj);
	return objs[objs.size() - 1];
}

glObject& GLObjectManager::addCone2(float radius, float height, int slices){
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
	obj.id = currentId;
	ids[currentId] = objs.size();
	newId();
	objs.push_back(obj);
	return objs[objs.size() - 1];
}

glObject& GLObjectManager::addPrism2(float baseR, float topR, float height, int slices){
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
	obj.id = currentId;
	ids[currentId] = objs.size();
	newId();
	objs.push_back(obj);
	return objs[objs.size() - 1];
}

glObject& GLObjectManager::addPrism(float radius, float height, int slices){
	return addPrism2(radius, radius, height, slices);
}

glObject& GLObjectManager::addCircle(float radius, int slices){
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
	obj.id = currentId;
	ids[currentId] = objs.size();
	newId();
	objs.push_back(obj);
	return objs[objs.size() - 1];
}

glObject& GLObjectManager::addPlane(float len, int slices){
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
	obj.id = currentId;
	ids[currentId] = objs.size();
	setConnectivity(obj);
	for (int i = 0; i < obj.planeSize; ++i)
		calPlane(obj, obj.planes[i]);
	newId();
	objs.push_back(obj);
	return objs[objs.size() - 1];
}

void GLObjectManager::drawAll(){
	for (int i = 0; i < objs.size(); ++i){

		drawGLObject(objs[i]);
	}
}







void GLObjectManager::drawCube(Vec3f center, Vec3f size){
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glScalef(size[0], size[1], size[2]);
	glutSolidCube(1.0);
	glPopMatrix();
}
void GLObjectManager::drawSphere(Vec3f center, Vec3f size){
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glScalef(size[0], size[1], size[2]);
	glutSolidSphere(1.0, 16, 16);
	glPopMatrix();
}

void GLObjectManager::drawCylinder(Vec2f size, Vec3f center, int slices){
	drawCylinder2(size[0], size[0], size[1], center, slices);
}
void GLObjectManager::drawCylinder2(float baseR, float topR, float height, Vec3f center, int slices){
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

void GLObjectManager::drawCone(float radius, float height, int slices, Vec3f center){
	drawCylinder2(radius, 0, height, center, slices);
}
void GLObjectManager::drawCone2(float radius, float height, int slices, Vec3f center){
	drawPrism2(radius, 0, height, slices, center);
}

void GLObjectManager::drawPrism(float radius, float height, int slices, Vec3f baseCenter){
	drawPrism2(radius, radius, height, slices, baseCenter);
}
void GLObjectManager::drawPrism2(float baseR, float topR, float height, int slices, Vec3f baseCenter){
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
void GLObjectManager::drawPrismSide(float baseR, float topR, float height, int slices){
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

void GLObjectManager::drawPlane(float length, int dir, Vec3f center){
	drawCircle(length / sqrt(2), 4, dir, center);
}
void GLObjectManager::drawCircle(float radius, int slices, int dir, Vec3f center){
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
