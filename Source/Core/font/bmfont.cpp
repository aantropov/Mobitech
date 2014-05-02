#include "../Mobitech.h"
#include "../Renderer.h"
#include "../Resources.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include "bmfont.h"

#pragma warning (disable : 4996)

char *replace_str(const char *str, char *orig, char *rep)
{
  static char buffer[BUFFER_LENGTH];
  const char *p = strstr(str, orig);

  strncpy(buffer, str, p-str);
  buffer[p-str] = '\0';

  sprintf(buffer + (p-str), "%s%s", rep, p + strlen(orig));

  return buffer;
}

typedef struct 
{
	float x,y;
    float texx,texy;
    float r,g,b,a;
} vlist;

vlist font_vertices_buffer[2048*4];
unsigned short font_indices_buffer[2048*4];

bool BMFont::ParseFont(const char *fontfile)
{
    AssetFile file(this->resource_factory, fontfile);
    unsigned int size = file.GetFileSize();
    char *file_buffer = new char[size];
    file.Read(file_buffer, 1, size);

    std::stringstream stream;
    stream << file_buffer;
    //stream.rdbuf()->pubsetbuf((char*)file_buffer, size);

    //std::ifstream stream(std::string(file_buffer)); 
	std::string line;
	std::string read, key, value;
	std::size_t i;
    
	KearningInfo info;
	CharDescriptor descriptor;
        
	while(!stream.eof())
	{
		std::stringstream line_stream;

		std::getline(stream, line);
		line_stream << line;

		//read the line's type
		line_stream >> read;
		if(read == "common")
		{
			//this holds common data
			while(!line_stream.eof())
			{
				std::stringstream Converter;
				line_stream >> read;
				i = read.find( '=');
				key = read.substr( 0, i);
				value = read.substr(i + 1);

				//assign the correct value
				Converter << value;
				if(key == "lineHeight")
				    Converter >> line_height;
				else if( key == "base")
				    Converter >> base;
				else if( key == "scaleW")
				    Converter >> width;
				else if( key == "scaleH")
				    Converter >> height;
				else if( key == "pages")
				    Converter >> pages;
				else if( key == "outline")
				    Converter >> outline;
			}
		}		
		else if(read == "char")
		{
			int CharID = 0;
			while( !line_stream.eof())
			{
				std::stringstream Converter;
				line_stream >> read;
				i = read.find('=');
				key = read.substr(0, i);
				value = read.substr(i+1);

				Converter << value;
				if( key == "id")
				    Converter >> CharID;
				else if( key == "x")
				    Converter >> descriptor.x;
				else if( key == "y")
				    Converter >> descriptor.y;  
				else if( key == "width")
				    Converter >> descriptor.width;
				else if( key == "height")
					Converter >> descriptor.height;         
				else if( key == "xoffset")
					Converter >> descriptor.XOffset;         
				else if( key == "yoffset")
					Converter >> descriptor.YOffset;        
				else if( key == "xadvance")
					Converter >> descriptor.XAdvance;
				else if( key == "page")
					Converter >> descriptor.Page;
			}
			
         	chars.insert(std::map<int,CharDescriptor>::value_type(CharID, descriptor));
			
		}
		else if( read == "kernings")
		{
			while( !line_stream.eof())
			{
				std::stringstream Converter;
				line_stream >> read;
				i = read.find( '=');
				key = read.substr( 0, i);
				value = read.substr( i + 1);

				Converter << value;
				if( key == "count")
				    Converter >> kern_count;
			}
		}
		else if( read == "kerning")
		{
			while( !line_stream.eof())
			{
				std::stringstream Converter;
				line_stream >> read;
				i = read.find( '=');
				key = read.substr( 0, i);
				value = read.substr( i + 1);

				//assign the correct value
				Converter << value;
				if( key == "first")
				{Converter >> info.First;}
				
				else if( key == "second")
				{Converter >> info.Second;  }
				
				else if( key == "amount")
				{Converter >> info.Amount;}
 			}
			kearn.push_back(info);
		}
	}

	//stream.close();    
    if(file_buffer != NULL)
        delete[] file_buffer;

	return true;
}

int BMFont::GetKerningPair(int first, int second) const
{
	 if(kern_count)	 
	 	 for(int j = 0; j < kern_count;  j++)
			if(kearn[j].First == first && kearn[j].Second == second)
				 return kearn[j].Amount;
return 0;
}

float BMFont::GetStringWidth(const char *str)
{
  float total = 0;
  CharDescriptor  *f;

  for(int i = 0; i != strlen(str); i++)
  { 
	  f = &chars[str[i]];
      total += f->XAdvance;
  }
  return total * fscale;
}

bool BMFont::Load(const string font_file)
{
    char* buf = replace_str(font_file.c_str(), ".txt", ".png");
   // atlas = dynamic_cast<Texture*>(rf->Load(buf, RT_TEXTURE));
    
    atlas = dynamic_cast<Texture*>(resource_factory->Load(buf, RT_TEXTURE));
    atlas->name = "texture";
    shader = resource_factory->Load(ASSETS_ROOT + "Shaders\\font_diffuse.vs", ASSETS_ROOT + "Shaders\\diffuse.ps");
        
    ParseFont(font_file.c_str());
	kern_count = (int)kearn.size();
	return true;
}

void BMFont::RenderString(ShaderProgram *shader, int len) const
{
   glVertexAttribPointer(shader->attribute_locations.color, 4, GL_FLOAT, GL_FALSE, sizeof(vlist) , &font_vertices_buffer[0].r);
   glEnableVertexAttribArray(shader->attribute_locations.color);

   glVertexAttribPointer(shader->attribute_locations.position, 2, GL_FLOAT, GL_FALSE, sizeof(vlist), &font_vertices_buffer[0].x);
   glEnableVertexAttribArray(shader->attribute_locations.position);

   glVertexAttribPointer(shader->attribute_locations.texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vlist), &font_vertices_buffer[0].texx);
   glEnableVertexAttribArray(shader->attribute_locations.texcoords);
   
   glDrawElements(GL_TRIANGLES, len*6, GL_UNSIGNED_SHORT, font_indices_buffer);
   //glDrawArrays(GL_QUADS, 0, len * 4);
 
   glDisableVertexAttribArray(shader->attribute_locations.color);
   glDisableVertexAttribArray(shader->attribute_locations.texcoords);
   glDisableVertexAttribArray(shader->attribute_locations.position);
}

void BMFont::Print(float x, float y, mat4 model, const char *text)
{
	float CurX = (float) x;
	float CurY = (float) y;
	float DstX = 0.0;
	float DstY = 0.0;
	int Flen;
	
    float adv = (float) 1.0/width;
	CharDescriptor  *f;
	va_list		ap;

    Renderer::GetInstance()->BindShaderProgram(shader);
    Renderer::GetInstance()->SetupCameraForShaderProgram(shader, model);
    Renderer::GetInstance()->BindTexture(atlas, 0);
	unsigned char *color = (unsigned char*)&fcolor;
	
	y = y + line_height;
    Flen = strlen(text);

	for(int i = 0; i != Flen; ++i)
	{
        //00 10 11     11 01 00
         /*font_indices_buffer[i*6] = 3;
         font_indices_buffer[i*6+1] = 2;
         font_indices_buffer[i*6+2] = 1;
         font_indices_buffer[i*6+3] = 1;
         font_indices_buffer[i*6+4] = 0;
         font_indices_buffer[i*6+5] = 3;
         */

         font_indices_buffer[i*6] = i*4 + 3;
         font_indices_buffer[i*6+1] = i*4 + 1;
         font_indices_buffer[i*6+2] = i*4 + 2;
         font_indices_buffer[i*6+3] = i*4 + 3;
         font_indices_buffer[i*6+4] = i*4 + 0;
         font_indices_buffer[i*6+5] = i*4 + 1;

		 f = &chars[text[i]];

		 CurX = x + f->XOffset;
		 CurY = y - f->YOffset;
		 DstX = CurX + f->width;
	     DstY = CurY - f->height;
		 			    
         //0,1 Texture Coord
		 font_vertices_buffer[i*4].texx = adv*f->x;
		 font_vertices_buffer[i*4].texy = (float)(adv * (f->y + f->height));
         font_vertices_buffer[i*4].x = (float) fscale * CurX;
		 font_vertices_buffer[i*4].y = (float) fscale * DstY;
		 font_vertices_buffer[i*4].r = ((float)color[0]) / 256.0f;
		 font_vertices_buffer[i*4].g = ((float)color[1]) / 256.0f;
		 font_vertices_buffer[i*4].b = ((float)color[2]) / 256.0f;
		 font_vertices_buffer[i*4].a = ((float)color[3]) / 256.0f;

		 //1,1 Texture Coord
		 font_vertices_buffer[(i*4)+1].texx = adv*(f->x+f->width);
		 font_vertices_buffer[(i*4)+1].texy = (float)(adv * (f->y + f->height));
         font_vertices_buffer[(i*4)+1].x = (float) fscale * DstX;
		 font_vertices_buffer[(i*4)+1].y = (float) fscale * DstY;
		 font_vertices_buffer[(i*4)+1].r = ((float)color[0]) / 256.0f;
		 font_vertices_buffer[(i*4)+1].g = ((float)color[1]) / 256.0f;
		 font_vertices_buffer[(i*4)+1].b = ((float)color[2]) / 256.0f;
		 font_vertices_buffer[(i*4)+1].a = ((float)color[3]) / 256.0f;

		 //1,0 Texture Coord
		 font_vertices_buffer[(i*4)+2].texx = adv*(f->x+f->width);
		 font_vertices_buffer[(i*4)+2].texy = (float)(adv*f->y);
         font_vertices_buffer[(i*4)+2].x = (float) fscale * DstX;
		 font_vertices_buffer[(i*4)+2].y = (float) fscale * CurY;
		 font_vertices_buffer[(i*4)+2].r = ((float)color[0]) / 256.0f;
		 font_vertices_buffer[(i*4)+2].g = ((float)color[1]) / 256.0f;
		 font_vertices_buffer[(i*4)+2].b = ((float)color[2]) / 256.0f;
		 font_vertices_buffer[(i*4)+2].a = ((float)color[3]) / 256.0f;

		 //0,0 Texture Coord
		 font_vertices_buffer[(i*4)+3].texx = adv*f->x;
		 font_vertices_buffer[(i*4)+3].texy = (float)(adv*f->y);
         font_vertices_buffer[(i*4)+3].x = (float) fscale * CurX;
		 font_vertices_buffer[(i*4)+3].y = (float) fscale * CurY;
		 font_vertices_buffer[(i*4)+3].r = ((float)color[0]) / 256.0f;
		 font_vertices_buffer[(i*4)+3].g = ((float)color[1]) / 256.0f;
		 font_vertices_buffer[(i*4)+3].b = ((float)color[2]) / 256.0f;
		 font_vertices_buffer[(i*4)+3].a = ((float)color[3]) / 256.0f;

		 if(Flen > 1 && i < Flen)
			 x += GetKerningPair(text[i],text[i+1]);
		  
		 x +=  f->XAdvance;
    }
   RenderString(shader, strlen(text));
}

void BMFont::Print(mat4 model, const char *fmt, ...)
{
    va_list		ap;
    char text[512] = "";

	if(fmt == NULL)
		return;

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);		

	Print(0.0f, 0.0f, model, text);
}

void BMFont::Print(float x, float y, const char *fmt, ...)
{
	va_list		ap;
    char text[512] = "";

	if(fmt == NULL)
		return;

	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);		

    Print(x, y, mat4_identity, text);
}

void BMFont::PrintCenter(float y, const char *string)
{  
	int x = 0;
	CharDescriptor  *f;		 
	
    int window_width = Renderer::GetInstance()->GetWidth();
    int len = strlen(string);

	for(int i = 0; i != len; ++i)
	{
	    f = &chars[string[i]];
		if(len > 1 && i < len)
		   x += GetKerningPair(string[i],string[i+1]);		
		x += f->XAdvance;
	}
	Print((float)(window_width/2) - (x/2) , y, string);
}

BMFont::~BMFont()
{
	chars.clear();
	kearn.clear();	
}