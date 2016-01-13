#include "globject.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "glut.h"
//#include "glext.h"
glObject::glObject(){
	for (int i = 0; i < 3; ++i){
		r[i] = t[i] = 0;
		s[i] = 1.0;
	}
	hasTex = false;
	materialFMap[GL_AMBIENT] = this->tex.ambient;
	materialFMap[GL_DIFFUSE] = this->tex.diffuse;
	materialFMap[GL_SPECULAR] = this->tex.specular;
	materialFMap[GL_EMISSION] = this->tex.emission;
}

float* glObject::getMaterialfv(int para){
	if (para == GL_DIFFUSE)return tex.diffuse;
	else if (para == GL_AMBIENT)return tex.ambient;
	else if (para == GL_SPECULAR)return tex.specular;
	else if (para == GL_EMISSION)return tex.emission;
		return materialFMap[para];
}

void setConnectivity(glObject &o){
	//±éÀúÃ¿Ò»¸öÃæ
	for (int i = 0; i < o.planeSize - 1; ++i){
		for (int j = i + 1; j < o.planeSize; ++j){
			//±éÀúÃ¿Ò»¸öµã
			for (int ki = 0; ki < 3; ++ki){
				if (!o.planes[i].neighbor[ki])
					for (int kj = 0; kj < 3; ++kj){
					int p1i = ki, p1j = kj;
					int p2i = (ki + 1) % 3;
					int p2j = (kj + 1) % 3;

					p1i = o.planes[i].p[p1i];
					p2i = o.planes[i].p[p2i];
					p1j = o.planes[j].p[p1j];
					p2j = o.planes[j].p[p2j];

					sPoint p11 = o.points[p1i];
					sPoint p12 = o.points[p2i];
					sPoint p21 = o.points[p1j];
					sPoint p22 = o.points[p2j];

					float len1 = sqrt((p11.x - p21.x) * (p11.x - p21.x) +
						(p11.y - p21.y) * (p11.y - p21.y) +
						(p11.z - p21.z) * (p11.z - p21.z));
					float len2 = sqrt((p12.x - p22.x) * (p12.x - p22.x) +
						(p12.y - p22.y) * (p12.y - p22.y) +
						(p12.z - p22.z) * (p12.z - p22.z));
					float len3 = sqrt((p11.x - p22.x) * (p11.x - p22.x) +
						(p11.y - p22.y) * (p11.y - p22.y) +
						(p11.z - p22.z) * (p11.z - p22.z));
					float len4 = sqrt((p12.x - p21.x) * (p12.x - p21.x) +
						(p12.y - p21.y) * (p12.y - p21.y) +
						(p12.z - p21.z) * (p12.z - p21.z));
					//p1i = ((p1i + p2i) - abs(p1i - p2i)) / 2;
					//p2i = ((p1i + p2i) + abs(p1i - p2i)) / 2;
					//p1j = ((p1j + p2j) - abs(p1j - p2j)) / 2;
					//p2j = ((p1j + p2j) + abs(p1j - p2j)) / 2;


					if (len1 < 1e-5 && len2 < 1e-5 || len3 < 1e-5 && len4 < 1e-5){  //they are neighbours
						o.planes[i].neighbor[ki] = j + 1;
						o.planes[j].neighbor[kj] = i + 1;
					}
					}
			}
		}
	}
}

void calPlane(glObject o, sPlane &plane){
	sPoint v[4];
	for (int i = 0; i < 3; ++i){
		v[i + 1].x = o.points[plane.p[i]].x;
		v[i + 1].y = o.points[plane.p[i]].y;
		v[i + 1].z = o.points[plane.p[i]].z;
	}
	plane.PlaneEq.a = v[1].y*(v[2].z - v[3].z) + v[2].y*(v[3].z - v[1].z) + v[3].y*(v[1].z - v[2].z);
	plane.PlaneEq.b = v[1].z*(v[2].x - v[3].x) + v[2].z*(v[3].x - v[1].x) + v[3].z*(v[1].x - v[2].x);
	plane.PlaneEq.c = v[1].x*(v[2].y - v[3].y) + v[2].x*(v[3].y - v[1].y) + v[3].x*(v[1].y - v[2].y);
	plane.PlaneEq.d = -(v[1].x*(v[2].y*v[3].z - v[3].y*v[2].z) +
		v[2].x*(v[3].y*v[1].z - v[1].y*v[3].z) +
		v[3].x*(v[1].y*v[2].z - v[2].y*v[1].z));
}

void drawGLObject(glObject o){
	glLoadName(o.id);
	if (o.hasTex){
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, o.tex.texture.texID);
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat*)o.tex.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat*)o.tex.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat*)o.tex.specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat*)o.tex.emission);
	glShadeModel(GL_SMOOTH);
	glPushMatrix();
	glTranslatef(o.t[0], o.t[1], o.t[2]);
	glRotatef(o.r[2], 0, 0, 1);
	glRotatef(o.r[1], 0, 1, 0);
	glRotatef(o.r[0], 1, 0, 0);
	glScalef(o.s[0], o.s[1], o.s[2]);
	for (int i = 0; i < o.planeSize; ++i){
		//if (o.planes[i].psize == 3)
		glBegin(GL_TRIANGLES);
		//else
		//	glBegin(GL_QUADS);

		for (int j = 0; j < 3; ++j){
			glNormal3f(o.planes[i].normal[j].x, o.planes[i].normal[j].y, o.planes[i].normal[j].z);
			if (o.hasTex)
				glTexCoord2f(o.texPoints[o.planes[i].tex[j]][0], o.texPoints[o.planes[i].tex[j]][1]);
			glVertex3f(o.points[o.planes[i].p[j]].x, o.points[o.planes[i].p[j]].y, o.points[o.planes[i].p[j]].z);
		}
		glEnd();
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
}
void castShadow(glObject &o, float *lp){
	sPoint v1, v2;
	float side;
	for (int i = 0; i < o.planeSize; ++i){
		//¼ÆËã¹âÔ´Î»ÓÚÆ½ÃæÄÄÒ»²à
		side = o.planes[i].PlaneEq.a * lp[0] +
			o.planes[i].PlaneEq.b * lp[1] +
			o.planes[i].PlaneEq.c * lp[2] +
			o.planes[i].PlaneEq.d * lp[3];
		if (side > 0)
			o.planes[i].visible = true;
		else
			o.planes[i].visible = false;
	}

	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_STENCIL_TEST);
	glColorMask(0, 0, 0, 0);
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	for (int i = 0; i < o.planeSize; ++i){
		if (o.planes[i].visible){
			for (int j = 0; j < 3; ++j){
				int k = o.planes[i].neighbor[j];
				if ((!k) || !o.planes[k - 1].visible){
					int p1 = o.planes[i].p[j];
					int jj = (j + 1) % 3;
					int p2 = o.planes[i].p[jj];

					v1.x = (o.points[p1].x - lp[0]) * 100;
					v1.y = (o.points[p1].y - lp[1]) * 100;
					v1.z = (o.points[p1].z - lp[2]) * 100;

					v2.x = (o.points[p2].x - lp[0]) * 100;
					v2.y = (o.points[p2].y - lp[1]) * 100;
					v2.z = (o.points[p2].z - lp[2]) * 100;

					glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(o.points[p1].x,
						o.points[p1].y,
						o.points[p1].z);
					glVertex3f(o.points[p1].x + v1.x,
						o.points[p1].y + v1.y,
						o.points[p1].z + v1.z);

					glVertex3f(o.points[p2].x,
						o.points[p2].y,
						o.points[p2].z);
					glVertex3f(o.points[p2].x + v2.x,
						o.points[p2].y + v2.y,
						o.points[p2].z + v2.z);
					glEnd();
				}
			}
		}
	}

	glFrontFace(GL_CW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	for (int i = 0; i < o.planeSize; i++){
		if (o.planes[i].visible)
			for (int j = 0; j < 3; j++){
			int k = o.planes[i].neighbor[j];
			if ((!k) || (!o.planes[k - 1].visible)){
				// here we have an edge, we must draw a polygon
				int p1 = o.planes[i].p[j];
				int jj = (j + 1) % 3;
				int p2 = o.planes[i].p[jj];

				//calculate the length of the vector
				v1.x = (o.points[p1].x - lp[0]) * 100;
				v1.y = (o.points[p1].y - lp[1]) * 100;
				v1.z = (o.points[p1].z - lp[2]) * 100;

				v2.x = (o.points[p2].x - lp[0]) * 100;
				v2.y = (o.points[p2].y - lp[1]) * 100;
				v2.z = (o.points[p2].z - lp[2]) * 100;

				//draw the polygon
				glBegin(GL_TRIANGLE_STRIP);
				glVertex3f(o.points[p1].x,
					o.points[p1].y,
					o.points[p1].z);
				glVertex3f(o.points[p1].x + v1.x,
					o.points[p1].y + v1.y,
					o.points[p1].z + v1.z);

				glVertex3f(o.points[p2].x,
					o.points[p2].y,
					o.points[p2].z);
				glVertex3f(o.points[p2].x + v2.x,
					o.points[p2].y + v2.y,
					o.points[p2].z + v2.z);
				glEnd();
			}
			}
	}

	glFrontFace(GL_CCW);
	glColorMask(1, 1, 1, 1);

	//draw a shadowing rectangle covering the entire screen

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	glShadeModel(GL_SMOOTH);
}

void drawShadow(){
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilFunc(GL_NOTEQUAL, 0, 0xffffffffL);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(-0.1f, 0.1f, -0.10f);
	glVertex3f(-0.1f, -0.1f, -0.10f);
	glVertex3f(0.1f, 0.1f, -0.10f);
	glVertex3f(0.1f, -0.1f, -0.10f);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glPopMatrix();

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	glShadeModel(GL_SMOOTH);
}

void VMatMult(GLmatrix16f M, GLvector4f v)
{
	GLfloat res[4];										// Hold Calculated Results
	res[0] = M[0] * v[0] + M[4] * v[1] + M[8] * v[2] + M[12] * v[3];
	res[1] = M[1] * v[0] + M[5] * v[1] + M[9] * v[2] + M[13] * v[3];
	res[2] = M[2] * v[0] + M[6] * v[1] + M[10] * v[2] + M[14] * v[3];
	res[3] = M[3] * v[0] + M[7] * v[1] + M[11] * v[2] + M[15] * v[3];
	v[0] = res[0];										// Results Are Stored Back In v[]
	v[1] = res[1];
	v[2] = res[2];
	v[3] = res[3];										// Homogenous Coordinate
}

int ReadObject(char *st, glObject *o){
	FILE *file;
	unsigned int i;

	file = fopen(st, "r");
	if (!file) return false;
	//points
	fscanf(file, "%d", &(o->pointSize));
	o->points = new sPoint[o->pointSize];
	for (i = 1; i <= o->pointSize; i++){
		fscanf(file, "%f", &(o->points[i].x));
		fscanf(file, "%f", &(o->points[i].y));
		fscanf(file, "%f", &(o->points[i].z));
	}
	//planes

	fscanf(file, "%d", &(o->planeSize));
	o->planes = new sPlane[o->planeSize];
	for (i = 0; i<o->planeSize; i++){
		o->planes[i].nsize = 0;
		o->planes[i].psize = 3;
		o->planes[i].tsize = 0;
		fscanf(file, "%d", &(o->planes[i].p[0]));
		fscanf(file, "%d", &(o->planes[i].p[1]));
		fscanf(file, "%d", &(o->planes[i].p[2]));


		fscanf(file, "%f", &(o->planes[i].normal[0].x));
		fscanf(file, "%f", &(o->planes[i].normal[0].y));
		fscanf(file, "%f", &(o->planes[i].normal[0].z));
		fscanf(file, "%f", &(o->planes[i].normal[1].x));
		fscanf(file, "%f", &(o->planes[i].normal[1].y));
		fscanf(file, "%f", &(o->planes[i].normal[1].z));
		fscanf(file, "%f", &(o->planes[i].normal[2].x));
		fscanf(file, "%f", &(o->planes[i].normal[2].y));
		fscanf(file, "%f", &(o->planes[i].normal[2].z));
	}
	return true;
}

void addShadowObject(glObject &o, Light light){
	for (int i = 0; i < light.light_num.size(); ++i){
		GLmatrix16f Minv;
		GLvector4f wlp, lp;
		glPushMatrix();
		glLoadIdentity();
		glScalef(1.0 / o.s[0], 1.0 / o.s[1], 1.0 / o.s[2]);
		glRotatef(-o.r[0], 1, 0, 0);
		glRotatef(-o.r[1], 0, 1, 0);
		glRotatef(-o.r[2], 0, 0, 1);

		glGetFloatv(GL_MODELVIEW_MATRIX, Minv);
		lp[0] = light.pLight[i].p[0];
		lp[1] = light.pLight[i].p[1];
		lp[2] = light.pLight[i].p[2];
		lp[3] = light.pLight[i].p[3];
		VMatMult(Minv, lp);
		glTranslatef(-o.t[0], -o.t[1], -o.t[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, Minv);
		wlp[0] = 0.0f;
		wlp[1] = 0.0f;
		wlp[2] = 0.0f;
		wlp[3] = 1.0f;
		VMatMult(Minv, wlp);
		lp[0] += wlp[0];
		lp[1] += wlp[1];
		lp[2] += wlp[2];
		glPopMatrix();
		glPushMatrix();
		glTranslatef(o.t[0], o.t[1], o.t[2]);

		glRotatef(o.r[2], 0, 0, 1);
		glRotatef(o.r[1], 0, 1, 0);
		glRotatef(o.r[0], 1, 0, 0);
		glScalef(o.s[0], o.s[1], o.s[2]);
		/*drawGLObject(o);*/
		castShadow(o, lp);
		glPopMatrix();
	}

}