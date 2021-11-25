#include "Model.h"
#include "lib3ds/mesh.h"
#include "lib3ds/node.h"
#include "lib3ds/material.h"
#include "lib3ds/matrix.h"
#include "lib3ds/vector.h"
#include "lib3ds/light.h"
#include <QString>
#include <QImage>

#include "Common.h"
#include "Room3DApplication.h"
#include "Room3DItemMgr.h"

CModel::CModel(void)
{
	ZeroMemory(m_strFileName ,MAX_PATH_LEN);
	memset(&m_vCenter, 0x00, sizeof(m_vCenter));
	memset(&m_vSize, 0x00, sizeof(m_vSize));
	memset(&m_sBB, 0, sizeof(BoundBox));

#if USE_MODEL_DISP_LIST
	m_nDispList = 0;
#endif

	m_pFile = NULL;
}

CModel::~CModel(void)
{
	if(m_pFile)
	{
		lib3ds_file_free(m_pFile);
		m_pFile = NULL;
	}

#if USE_MODEL_DISP_LIST
	if(m_nDispList)
	{
		glDeleteLists(m_nDispList, 1);
	}
#endif
}

bool CModel::Load(char* pFileName)
{
	bool bRet = false;

	if(pFileName)
	{
		strcpy(m_strFileName, pFileName);
		m_pFile=lib3ds_file_load(m_strFileName);
		if(m_pFile)
		{
			if( !m_pFile->nodes )
			{
				Lib3dsMesh *mesh;
				Lib3dsNode *node;

				for(mesh = m_pFile->meshes; mesh != NULL; mesh = mesh->next)
				{
					node = lib3ds_node_new_object();
					strcpy(node->name, mesh->name);
					node->parent_id = LIB3DS_NO_PARENT;
					lib3ds_file_insert_node(m_pFile, node);
				}
			}

			LoadTextures();
			lib3ds_file_eval(m_pFile, 1.0f);
			CalcBoundingBox();
			CreateDefaultLight();
			lib3ds_file_eval(m_pFile, 0.f);
			bRet = true;
		}
		else
		{
			MessageBoxA(NULL, "Error : Impossible to load model!", "M3DS Error", MB_ICONERROR);
		}
	}

	return bRet;
}

void CModel::LoadTextures()
{
	if(m_pFile == NULL)
		return;

	Lib3dsMaterial *pMaterial = m_pFile->materials;

	while(pMaterial)
	{
		LoadTexture(&pMaterial->texture1_map);
		pMaterial = pMaterial->next;
	}
}

void CModel::LoadTexture(Lib3dsTextureMap* pTextureMap)
{
	if(pTextureMap == NULL)
		return;

	pTextureMap->user.i = -1;

	if(pTextureMap->name[0])
	{
		char strFilePath[MAX_PATH_LEN] = {0,};
		char* pLastSlash = strrchr(m_strFileName, '\\');
		if(pLastSlash)
		{
			strncpy(strFilePath, m_strFileName, (pLastSlash - m_strFileName + 1));
			strcat(strFilePath, pTextureMap->name);
		}
		else
		{
			strcpy(strFilePath, pTextureMap->name);
		}

		CTexturePool* pTexturePool = g_pApp->GetDocument()->GetItemMgr()->GetTexturePool();
		if(pTexturePool)
		{
			pTextureMap->user.i = pTexturePool->Load(strFilePath);
		}
	}
}

void CModel::CalcBoundingBox()
{
	if(m_pFile == NULL)
		return;

	Lib3dsVector min, max;
	lib3ds_file_bounding_box_of_nodes(m_pFile, LIB3DS_TRUE, LIB3DS_FALSE, LIB3DS_FALSE, min, max);

	vec3 vMin(min);
	vec3 vMax(max);
	m_sBB.m_vMin = vMin;
	m_sBB.m_vMax = vMax;

	m_vCenter = vMin + vMax;
	m_vCenter *= 0.5f;
	m_vSize = vMax - vMin;
}

void CModel::CreateDefaultLight()
{
	if(m_pFile == NULL)
		return;

	if (m_pFile->lights == NULL)
	{
		Lib3dsLight *light;
#if 1
		light = lib3ds_light_new("light0");
		light->spot_light = 0;
		light->see_cone = 0;
		light->color[0] = light->color[1] = light->color[2] = 0.5f;
		light->position[0] = m_vCenter.x;
		light->position[1] = m_vCenter.y;
		light->position[2] = m_vCenter.z;
		light->position[2] -= m_vSize.z * 1.5f;
		light->position[3] = 0.f;
		light->outer_range = 100.f;
		light->inner_range = 10.f;
		light->multiplier = 1.f;
		lib3ds_file_insert_light(m_pFile, light);

		light = lib3ds_light_new("light1");
		light->spot_light = 0;
		light->see_cone = 0;
		light->color[0] = light->color[1] = light->color[2] = 0.5f;
		light->position[0] = m_vCenter.x;
		light->position[1] = m_vCenter.y;
		light->position[2] = m_vCenter.z;
		light->position[2] += m_vSize.z * 1.5f;
		light->position[3] = 0.f;
		light->outer_range = 100.f;
		light->inner_range = 10.f;
		light->multiplier = 1.f;
		lib3ds_file_insert_light(m_pFile, light);

		light = lib3ds_light_new("light2");
		light->spot_light = 0;
		light->see_cone = 0;
		light->color[0] = light->color[1] = light->color[2] = 0.5f;
		light->position[0] = m_vCenter.x;
		light->position[0] -= m_vSize.x * 1.5f;
		light->position[1] = m_vCenter.y;
		light->position[2] = m_vCenter.z;
		light->position[3] = 0.f;
		light->outer_range = 100.f;
		light->inner_range = 10.f;
		light->multiplier = 1.f;
		lib3ds_file_insert_light(m_pFile, light);

		light = lib3ds_light_new("light3");
		light->spot_light = 0;
		light->see_cone = 0;
		light->color[0] = light->color[1] = light->color[2] = 0.5f;
		light->position[0] = m_vCenter.x;
		light->position[0] += m_vSize.x * 1.5f;
		light->position[1] = m_vCenter.y;
		light->position[2] = m_vCenter.z;
		light->position[3] = 0.f;
		light->outer_range = 100.f;
		light->inner_range = 10.f;
		light->multiplier = 1.f;
		lib3ds_file_insert_light(m_pFile, light);

		light = lib3ds_light_new("light4");
		light->spot_light = 0;
		light->see_cone = 0;
		light->color[0] = light->color[1] = light->color[2] = 0.5f;
		light->position[0] = m_vCenter.x;
		light->position[1] = m_vCenter.y;
		light->position[1] -= m_vSize.y * 1.5f;
		light->position[2] = m_vCenter.z;
		light->position[3] = 0.f;
		light->outer_range = 100.f;
		light->inner_range = 10.f;
		light->multiplier = 1.f;
		lib3ds_file_insert_light(m_pFile, light);

		light = lib3ds_light_new("light5");
		light->spot_light = 0;
		light->see_cone = 0;
		light->color[0] = light->color[1] = light->color[2] = 0.5f;
		light->position[0] = m_vCenter.x;
		light->position[1] = m_vCenter.y;
		light->position[1] += m_vSize.y * 1.5f;
		light->position[2] = m_vCenter.z;
		light->position[3] = 0.f;
		light->outer_range = 100.f;
		light->inner_range = 10.f;
		light->multiplier = 1.f;
		lib3ds_file_insert_light(m_pFile, light);
#else
		light = lib3ds_light_new("light0");
		light->spot_light = 0;
		light->see_cone = 0;
		light->color[0] = light->color[1] = light->color[2] = 0.6f;
		light->position[0] = m_vCenter.x + m_vSize.x * 0.75f;
		light->position[1] = m_vCenter.y + m_vSize.y * 1.f;
		light->position[2] = m_vCenter.z + m_vSize.z * 1.5f;
		light->position[3] = 0.f;
		light->outer_range = 100.f;
		light->inner_range = 10.f;
		light->multiplier = 1.f;
		lib3ds_file_insert_light(m_pFile, light);

		light = lib3ds_light_new("light1");
		light->spot_light = 0;
		light->see_cone = 0;
		light->color[0] = light->color[1] = light->color[2] = 0.3f;
		light->position[0] = m_vCenter.x - m_vSize.x;
		light->position[1] = m_vCenter.y - m_vSize.y;
		light->position[2] = m_vCenter.z + m_vSize.z * 0.75f;
		light->position[3] = 0.f;
		light->outer_range = 100.f;
		light->inner_range = 10.f;
		light->multiplier = 1.f;
		lib3ds_file_insert_light(m_pFile, light);

		light = lib3ds_light_new("light2");
		light->spot_light = 0;
		light->see_cone = 0;
		light->color[0] = light->color[1] = light->color[2] = 0.3f;
		light->position[0] = m_vCenter.x;
		light->position[1] = m_vCenter.y + m_vSize.y;
		light->position[2] = m_vCenter.z + m_vSize.z;
		light->position[3] = 0.f;
		light->outer_range = 100.f;
		light->inner_range = 10.f;
		light->multiplier = 1.f;
		lib3ds_file_insert_light(m_pFile, light);
#endif
	}
}

void CModel::Render(bool bAlpha)
{
	if(m_pFile == NULL)
		return;

#if USE_MODEL_DISP_LIST
	if(m_nDispList)
	{
		glCallList(m_nDispList);
	}
	else
	{
		m_nDispList = glGenLists(1);
		glNewList(m_nDispList, GL_COMPILE);
#endif

		SetLight();
		RenderNodes(bAlpha);

#if USE_MODEL_DISP_LIST
		glEndList();
	}
#endif
}

void CModel::LightUpdate(Lib3dsLight *pLight)
{
	if(m_pFile == NULL)
		return;

	Lib3dsNode *ln, *sn;

	ln = lib3ds_file_node_by_name(m_pFile, pLight->name, LIB3DS_LIGHT_NODE);
	sn = lib3ds_file_node_by_name(m_pFile, pLight->name, LIB3DS_SPOT_NODE);

	if( ln != NULL ) {
		memcpy(pLight->color, ln->data.light.col, sizeof(Lib3dsRgb));
		memcpy(pLight->position, ln->data.light.pos, sizeof(Lib3dsVector));
	}

	if( sn != NULL )
		memcpy(pLight->spot, sn->data.spot.pos, sizeof(Lib3dsVector));
}

void CModel::SetLight()
{
	if(m_pFile == NULL)
		return;

	GLfloat a[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat c[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat p[] = {0.0f, 0.0f, 0.0f, 1.0f};
	Lib3dsLight *pLight = m_pFile->lights;

	int li=GL_LIGHT0;
	while(pLight)
	{
		glEnable(li);
		LightUpdate(pLight);

		c[0] = pLight->color[0];
		c[1] = pLight->color[1];
		c[2] = pLight->color[2];
		glLightfv(li, GL_AMBIENT, a);
		glLightfv(li, GL_DIFFUSE, c);
		glLightfv(li, GL_SPECULAR, c);

		p[0] = pLight->position[0];
		p[1] = pLight->position[1];
		p[2] = pLight->position[2];
		glLightfv(li, GL_POSITION, p);

		if (pLight->spot_light)
		{
			p[0] = pLight->spot[0] - pLight->position[0];
			p[1] = pLight->spot[1] - pLight->position[1];
			p[2] = pLight->spot[2] - pLight->position[2];
			glLightfv(li, GL_SPOT_DIRECTION, p);
		}
		++li;
		pLight=pLight->next;
	}
}

void CModel::RenderNode(Lib3dsNode *pNode, bool bAlpha)
{
	if(pNode == NULL)
		return;

	int tex_mode = 0;

	CTexturePool* pTexturePool = g_pApp->GetDocument()->GetItemMgr()->GetTexturePool();
	// Render child nodes
	Lib3dsNode *pChildNode = pNode->childs;
	while(pChildNode != NULL)
	{
		RenderNode(pChildNode, bAlpha);
		pChildNode = pChildNode->next;
	}

	// Render node
	if (pNode->type==LIB3DS_OBJECT_NODE) 
	{
		Lib3dsObjectData *d;

		glPushMatrix();
		d=&pNode->data.object;
		glMultMatrixf(&pNode->matrix[0][0]);
		glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);

	    Lib3dsMesh *mesh;

		if (strcmp(pNode->name, "$$$DUMMY")==0) 
		{
		  return;
		}

		mesh = lib3ds_file_mesh_by_name(m_pFile, pNode->data.object.morph);
		if( mesh == NULL )
			mesh = lib3ds_file_mesh_by_name(m_pFile, pNode->name);

		if(mesh == NULL)
			return;

		unsigned p;
#if !CALC_NORMAL_AT_LOADING_TIME /*002-->calc vertex normal at loading time*/
		Lib3dsVector *normalL=(Lib3dsVector *)malloc(3*sizeof(Lib3dsVector)*mesh->faces);
#endif
		{
			Lib3dsMatrix M;
			lib3ds_matrix_copy(M, mesh->matrix);
			lib3ds_matrix_inv(M);
			glMultMatrixf(&M[0][0]);
		}

#if !CALC_NORMAL_AT_LOADING_TIME /*002-->calc vertex normal at loading time*/
		lib3ds_mesh_calculate_normals(mesh, normalL);
#endif
		for (p=0; p<mesh->faces; ++p) 
		{
			Lib3dsFace *f=&mesh->faceL[p];
			Lib3dsMaterial *mat=0;
			if (f->material[0]) 
			{
				mat=lib3ds_file_material_by_name(m_pFile, f->material);
			}

			if (mat) 
			{
				if(bAlpha)
				{
					if(mat->diffuse[3] >= 1.f)
					{
#if !CALC_NORMAL_AT_LOADING_TIME /*002-->calc vertex normal at loading time*/
						free(normalL);
#endif
						glPopMatrix();
						return;
					}
				}
				else
				{
					if(mat->diffuse[3] < 1.f)
					{
#if !CALC_NORMAL_AT_LOADING_TIME /*002-->calc vertex normal at loading time*/
						free(normalL);
#endif
						glPopMatrix();
						return;
					}
				}

				if( mat->two_sided )
					glDisable(GL_CULL_FACE);
				else
					glEnable(GL_CULL_FACE);

				//glDisable(GL_CULL_FACE);

				/* Texturing */
				if (mat->texture1_map.name[0])  /* texture map? */
				{
					Lib3dsTextureMap *tex = &mat->texture1_map;
					if (tex->user.i>=0) /* texture loaded? */
					{
						if(pTexturePool)
						{
							pTexturePool->SetTex(tex->user.i);
						}
						tex_mode = 1;
					}
					else
					{
						tex_mode = 0;
					}
				}
				glMaterialfv(GL_FRONT, GL_AMBIENT, mat->ambient);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
				glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
				glMaterialf(GL_FRONT, GL_SHININESS, pow(2, 10.0*mat->shininess));
			}
			else
			{
				if(bAlpha)
				{
#if !CALC_NORMAL_AT_LOADING_TIME /*002-->calc vertex normal at loading time*/
					free(normalL);
#endif
					glPopMatrix();
					return;
				}

				const Lib3dsRgba a={0.7, 0.7, 0.7, 1.0};
				const Lib3dsRgba d={0.7, 0.7, 0.7, 1.0};
				const Lib3dsRgba s={1.0, 1.0, 1.0, 1.0};
				glMaterialfv(GL_FRONT, GL_AMBIENT, a);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
				glMaterialfv(GL_FRONT, GL_SPECULAR, s);
				glMaterialf(GL_FRONT, GL_SHININESS, pow(2, 10.0*0.5));
				tex_mode = 0;
			}

			glBegin(GL_TRIANGLES);
			glNormal3fv(f->normal);

			for (int i=0; i<3; ++i)
			{
#if CALC_NORMAL_AT_LOADING_TIME /*002-->calc vertex normal at loading time*/
				glNormal3fv(mesh->normalL[3*p+i]);
#else
				glNormal3fv(normalL[3*p+i]);
#endif
				if (tex_mode) 
				{
					glTexCoord2fv(mesh->texelL[f->points[i]]);
				}
				glVertex3fv(mesh->pointL[f->points[i]].pos);
			}
			glEnd();

			if (tex_mode)
				glDisable(GL_TEXTURE_2D);
		}

#if !CALC_NORMAL_AT_LOADING_TIME /*002-->calc vertex normal at loading time*/
		free(normalL);
#endif
		glPopMatrix();
	}
}

void CModel::RenderNodes(bool bAlpha)
{
	if(m_pFile == NULL)
		return;

	Lib3dsNode *pNode = m_pFile->nodes;

	while(pNode != NULL)
	{
		RenderNode(pNode, bAlpha);
		pNode = pNode->next;
	}
}
