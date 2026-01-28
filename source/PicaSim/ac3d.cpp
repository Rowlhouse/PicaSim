#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib> //TODO: metti if con <malloc.h>
#include <math.h>

#include "ac3d.h"

#include "Helpers.h"

// String equality comparison macro (was in Marmalade)
#define streq(a, b) (strcmp((a), (b)) == 0)

static int line = 0;
static char buff[256];

//======================================================================================================================
void ConvertLine(char* buff, int line, bool compress)
{
    int key = line + 13;
    const int start = 32;
    const int end = 126;
    const int range = end - start;
    const int offset = compress ? (key % range) : -(key % range);
    while (*buff != 0)
    {
        int c = *buff;
        if (c >= start && c < end)
        {
            c -= start;
            c = (c + range + offset) % range;
            *buff = c + start;
        }
        ++buff;
    }
}
//======================================================================================================================
bool read_line(FILE *f, bool compressed)
{
    fgets(buff, 255, f); 
    if (compressed)
        ConvertLine(buff, line, false);
    line++;
    return true;
}


int tokc = 0;
char *tokv[256];

//======================================================================================================================
int get_tokens(char *s, int *argc, char *argv[])
    /** bung '\0' chars at the end of tokens and set up the array (tokv) and count (tokc)
    like argv argc **/
{
    char *p = s;
    char *st;
    char c;
    //int n;
    int tc;

    tc = 0;
    while ((c=*p) != 0)
    {
        if ((c != ' ') && (c != '\t') && (c != '\n') && ( c != 13))
        {
            if (c == '"')
            {
                c = *p++;
                st = p;
                while ((c = *p) && ((c != '"')&&(c != '\n')&& ( c != 13)) )
                {
                    if (c == '\\')
                        memmove(p, p+1, strlen(p));  // Safe: memmove handles overlapping memory
                    p++;
                }
                *p=0;
                argv[tc++] = st;
            }
            else
            {
                st = p;
                while ((c = *p) && ((c != ' ') && (c != '\t') && (c != '\n') && ( c != 13)) )
                    p++;
                *p=0;
                argv[tc++] = st;
            }      
        }
        p++;
    }

    *argc = tc;
    return(tc);
}


//======================================================================================================================
void new_object(ACObject& ob)
{
    ob.loc.x = ob.loc.y = ob.loc.z = 0.0;
    ob.creaseAngle = 0.0;
    ob.name.clear();
    ob.url.clear();
    ob.data.clear();
    ob.vertices.clear();
    ob.surfaces.clear();
    ob.textureName.clear();
    ob.texture_repeat_x = ob.texture_repeat_y = 1.0;
    ob.texture_offset_x = ob.texture_offset_y = 0.0;
    ob.mObjects.clear();
    ob.matrix[0] = 1;
    ob.matrix[1] = 0;
    ob.matrix[2] = 0;
    ob.matrix[3] = 0;
    ob.matrix[4] = 1;
    ob.matrix[5] = 0;
    ob.matrix[6] = 0;
    ob.matrix[7] = 0;
    ob.matrix[8] = 1;
}


//======================================================================================================================
void init_surface(ACSurface *s)
{
    s->vertref.clear();
    s->uvs.clear();
    s->flags = 0;
    s->mat = 0;
    s->normal.x = 0.0; s->normal.z = 0.0; s->normal.z = 0.0; 
}

//======================================================================================================================
void tri_calc_normal(Vector3 *v1, Vector3 *v2, Vector3 *v3, Vector3 *n)
{
    double len;

    n->x = (v2->y-v1->y)*(v3->z-v1->z)-(v3->y-v1->y)*(v2->z-v1->z);
    n->y = (v2->z-v1->z)*(v3->x-v1->x)-(v3->z-v1->z)*(v2->x-v1->x);
    n->z = (v2->x-v1->x)*(v3->y-v1->y)-(v3->x-v1->x)*(v2->y-v1->y);
    len = sqrt(n->x*n->x + n->y*n->y + n->z*n->z);

    if (len > 0)
    {
        n->x /= (float)len;
        n->y /= (float)len;
        n->z /= (float)len;  
    }
}

//======================================================================================================================
ACSurface *read_surface(FILE *f, ACSurface *s, ACObject *ob, bool compressed)
{
    char t[64];

    init_surface(s);

    while (!feof(f))
    {
        read_line(f, compressed);
        sscanf(buff, "%s", t);

        if (streq(t, "SURF"))
        {
            int flgs;

            if (get_tokens(buff, &tokc, tokv) != 2)
            {
                printf("SURF should be followed by one flags argument\n");
            }
            else
            {
                flgs = strtol(tokv[1], NULL, 0);
                s->flags = flgs;
            }
        }
        else if (streq(t, "mat"))
        {
            int mindx;

            sscanf(buff, "%s %d", t, &mindx);
            s->mat = mindx;
        }
        else if (streq(t, "refs"))
        {
            int num, n;
            int ind;
            float tx, ty;

            sscanf(buff, "%s %d", t, &num);        

            s->vertref.resize(num);
            s->uvs.resize(num);

            for (n = 0; n < num; n++)
            {
                read_line(f, compressed);
                sscanf(buff, "%d %f %f", &ind, &tx, &ty);
                s->vertref[n] = ind;
                s->uvs[n].u = tx;
                s->uvs[n].v = ty;
            }

            /** calc surface normal **/
            if (s->vertref.size() >= 3)
                tri_calc_normal((Vector3 *)&ob->vertices[s->vertref[0]], 
                (Vector3 *)&ob->vertices[s->vertref[1]], 
                (Vector3 *)&ob->vertices[s->vertref[2]], (Vector3 *)&s->normal);

            return(s);
        }
        else
        {
            printf("ignoring %s\n", t);
        }
    }
    return(NULL);
}

//======================================================================================================================
void ac_object_calc_vertex_normals(ACObject *ob)
{
    float cosCreaseAngle = FastCos(DegreesToRadians(ob->creaseAngle));
    for (size_t s = 0 ; s != ob->surfaces.size() ; ++s)
    {
        ACSurface& surface = ob->surfaces[s];
        const Vector3& n = surface.normal;
        surface.vertexNormals.resize(surface.vertref.size());
        for (size_t v = 0 ; v != surface.vertref.size() ; ++v)
        {
            size_t vertexIndex = surface.vertref[v];
            Vector3 accumulatedNormal = n;
            int found = 1;
            // Find all the other adjacent surfaces that have a normal within the crease angle
            for (size_t s1 = 0 ; s1 != ob->surfaces.size() ; ++s1)
            {
                if (s1 == s)
                    continue;
                ACSurface& surface1 = ob->surfaces[s1];
                Vector3 n1 = surface1.normal;
                float dot = n.x * n1.x + n.y * n1.y + n.z * n1.z;
                if (dot < cosCreaseAngle)
                    continue;
                for (size_t v1 = 0 ; v1 != surface1.vertref.size() ; ++v1)
                {
                    size_t vertexIndex1 = surface1.vertref[v1];
                    if (vertexIndex == vertexIndex1)
                    {
                        accumulatedNormal += n1;
                        ++found;
                        break;
                    }
                }
            }
            accumulatedNormal *= 1.0f/found;
            surface.vertexNormals[v] = accumulatedNormal;
        }
    }
}

//======================================================================================================================
int string_to_objecttype(char *s)
{
    if (streq("world", s))
        return(OBJECT_WORLD);
    if (streq("poly", s))
        return(OBJECT_NORMAL);
    if (streq("group", s))
        return(OBJECT_GROUP);
    if (streq("light", s))
        return(OBJECT_LIGHT);
    return(OBJECT_NORMAL);
}

//======================================================================================================================
bool ac_load_object(ACObject& ob, FILE *f, ACObject *parent, Materials& materials, bool compressed)
{
    char t[64];
    while (!feof(f))
    {
        read_line(f, compressed);

        sscanf(buff, "%s", t);

        if (streq(t, "MATERIAL"))
        {
            if (get_tokens(buff, &tokc, tokv) != 22)
            {
                printf("expected 21 params after \"MATERIAL\" - line %d\n", line);
            }
            else
            {
                float shi, tran;
                ACMaterial m;

                m.name = tokv[1];
                m.rgb.r = (float)atof(tokv[3]);
                m.rgb.g = (float)atof(tokv[4]);
                m.rgb.b = (float)atof(tokv[5]);
                m.rgb.a = 1.0f;

                m.ambient.r = (float)atof(tokv[7]);
                m.ambient.g = (float)atof(tokv[8]);
                m.ambient.b = (float)atof(tokv[9]);
                m.ambient.a = 1.0f;

                m.emissive.r = (float)atof(tokv[11]);
                m.emissive.g = (float)atof(tokv[12]);
                m.emissive.b = (float)atof(tokv[13]);
                m.emissive.a = 1.0f;

                m.specular.r = (float)atof(tokv[15]);
                m.specular.g = (float)atof(tokv[16]);
                m.specular.b = (float)atof(tokv[17]);
                m.specular.a = 1.0f;

                m.shininess = (float)atof(tokv[19]);
                m.transparency = (float)atof(tokv[21]);

                shi = (float)atof(tokv[6]);
                tran = (float)atof(tokv[7]);

                materials.push_back(m);
            }
        }
        else if (streq(t, "OBJECT"))
        {
            char type[32];
            char str[32];
            new_object(ob);

            sscanf(buff, "%s %s", str, type);

            ob.type = string_to_objecttype(type);
        }
        else if (streq(t, "data"))
        {
            if (get_tokens(buff, &tokc, tokv) != 2)
                printf("expected 'data <number>' at line %d\n", line);
            else
            {
                char *str;
                int len;

                len = atoi(tokv[1]);
                if (len > 0)
                {
                    str = (char *)malloc(len+1);
                    fread(str, len, 1, f);
                    str[len] = 0;
                    fscanf(f, "\n"); line++;
                    ob.data = str;
                    free(str);
                }
            }
        }
        else if (streq(t, "name"))
        {
            int numtok = get_tokens(buff, &tokc, tokv);
            if (numtok != 2)
            {
                printf("expected quoted name at line %d (got %d tokens)\n", line, numtok);
            }
            else
                ob.name = tokv[1];
        }
        else if (streq(t, "texture"))
        {
            if (get_tokens(buff, &tokc, tokv) != 2)
                printf("expected quoted texture name at line %d\n", line);

            else
            {
                ob.textureName = tokv[1];
            }
        }
        else if (streq(t, "texrep"))
        {
            if (get_tokens(buff, &tokc, tokv) != 3)
                printf("expected 'texrep <float> <float>' at line %d\n", line);
            else
            {
                ob.texture_repeat_x = (float)atof(tokv[1]);
                ob.texture_repeat_y = (float)atof(tokv[2]);
            }
        }
        else if (streq(t, "texoff"))
        {
            if (get_tokens(buff, &tokc, tokv) != 3)
                printf("expected 'texoff <float> <float>' at line %d\n", line);
            else
            {
              ob.texture_offset_x = (float)atof(tokv[1]);
              ob.texture_offset_y = (float)atof(tokv[2]);
            }
        }
        else if (streq(t, "rot"))
        {
            float r[9];
            char str2[5];
            int n;

            sscanf(buff, "%s %f %f %f %f %f %f %f %f %f", str2, 
                &r[0], &r[1], &r[2], &r[3], &r[4], &r[5], &r[6], &r[7], &r[8] );

            for (n = 0; n < 9; n++)
                ob.matrix[n] = r[n];

        }
        else if (streq(t, "loc"))
        {
            char str[5];
            sscanf(buff, "%s %f %f %f", str,
                &ob.loc.x, &ob.loc.y, &ob.loc.z);      
        }
        else if (streq(t, "crease"))
        {
            char str[8];
            sscanf(buff, "%s %f", str,
                &ob.creaseAngle);      
        }
        else if (streq(t, "url"))
        {
            if (get_tokens(buff, &tokc, tokv) != 2)
                printf("expected one arg to url at line %d (got %s)\n", line, tokv[0]);
            else
                ob.url = tokv[1];
        }
        else if (streq(t, "numvert"))
        {
            int num, n;
            char str[10];

            sscanf(buff, "%s %d", str, &num);

            if (num > 0)
            {
                ob.vertices.resize(num);;

                for (n = 0; n < num; n++)
                {
                    Vector3 p;
                    read_line(f, compressed);
                    sscanf(buff, "%f %f %f", &p.x, &p.y, &p.z);
                    ob.vertices[n] = p;
                }
            }
        }
        else if (streq(t, "numsurf"))
        {
            int num, n;
            char str[10];

            sscanf(buff, "%s %d", str, &num);
            if (num > 0)
            {
                ob.surfaces.resize(num);
                for (n = 0; n < num; n++)
                {
                    ACSurface *news = read_surface(f, &ob.surfaces[n], &ob, compressed);
                    if (news == NULL)
                    {
                        printf("error whilst reading surface at line: %d\n", line);
                        return(NULL);
                    }

                }
            }
        }
        else
        {
            if (streq(t, "kids")) /** 'kids' is the last token in an object **/
            {
                int num, n;

                sscanf(buff, "%s %d", t, &num);

                if (num != 0)
                {
                    ob.mObjects.resize(num);

                    for (n = 0; n < num; n++)
                    {
                        if (!ac_load_object(ob.mObjects[n], f, &ob, materials, compressed))
                        {
                            printf("error reading expected child object %d of %d at line: %d\n", n+1, num, line);
                            return(true);
                        }
                    }

                }
                return true;
            }
        }

    }
    return false;
}


//======================================================================================================================
void ac_calc_vertex_normals(ACObject *ob)
{
    ac_object_calc_vertex_normals(ob);
    for (size_t n = 0; n < ob->mObjects.size(); n++)
        ac_calc_vertex_normals(&ob->mObjects[n]);
}


//======================================================================================================================
bool ACLoadModel(ACModel& model, const char *fname)
{
    ACObject& ob = model.mObject;
    bool compressed = false;
    line = 0;

    FILE *f = fopen(fname, "rb");
    if (f == NULL)
    {
        IwAssertMsg(ROWLHOUSE, false, ("Failed to open file"));
        char cf[512];
        sprintf(cf, "%s.p", fname);
        f = fopen(cf, "rb");
        if (f == NULL)
        {
            IwAssertMsg(ROWLHOUSE, false, ("Failed to open file"));
            printf("can't open %s or %s\n", fname, cf);
            return false;
        }
        else
        {
            compressed = true;
        }
    }

    read_line(f, compressed);

    if (strncmp(buff, "AC3D", 4))
    {
        printf("ac_load_ac '%s' is not a valid AC3D file.", fname);
        fclose(f);
        return false;
    }

    bool ret = ac_load_object(ob, f, NULL, model.mMaterials, compressed);

    fclose(f);

    if (ret)
        ac_calc_vertex_normals(&ob);

    return(ret);
}

//======================================================================================================================
void ac_dump(ACObject *ob)
{
    printf("OBJECT name %s\nloc %f %f %f\nnum_vert %lu\nnum_surf %lu\n",
        ob->name.c_str(), ob->loc.x, ob->loc.y, ob->loc.z, (long unsigned) ob->vertices.size(), (long unsigned) ob->surfaces.size());


    for (size_t n=0; n < ob->vertices.size(); n++)
        printf("\tv %f %f %f\n", ob->vertices[n].x, ob->vertices[n].y, ob->vertices[n].z);

    for (size_t n=0; n < ob->surfaces.size(); n++)
    {
        ACSurface *s = &ob->surfaces[n];
        printf("surface %lu, %lu refs, mat %d\n", (long unsigned) n, (long unsigned) s->vertref.size(), s->mat);
    }

    for (size_t n = 0; n < ob->mObjects.size(); n++)
        ac_dump(&ob->mObjects[n]); 

}
