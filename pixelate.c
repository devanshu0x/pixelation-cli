#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<png.h>


char * err=NULL;
int PIXELATION_FACTOR=10;
/* return 1 if file name is correct 0 otherwise */
int correct_file_name(const char * filename){
    char * extension=".png";
    int filename_len=strlen(filename);
    int extension_len=strlen(extension);

    if(filename_len>extension_len){
        // strcmp(str1, str2) == 0 confirms that str1 and str2 are equal.
        return strcmp(filename +(filename_len-extension_len),extension)==0;
    }
    return 0;
}

void pixalate(png_bytep * row_pointers,int width,int height, int factor){
    for(int y=0;y<height;y+=factor){
        for(int x=0;x<width;x+=factor){
            // calculate avg of r,g,b respectively in block of factor*factor
            int r_total=0, g_total=0, b_total=0,count=0;
            for(int dy=0;dy<factor && y+dy<height;dy++){
                png_bytep row =row_pointers[y+dy];
                for(int dx=0;dx<factor && x+dx<width;dx++){
                    png_bytep pixel =&(row[(x+dx)*3]);
                    r_total+=pixel[0];
                    g_total+=pixel[1];
                    b_total+=pixel[2];
                    count++;
                }
            }

            png_byte r_avg =r_total/count;
            png_byte g_avg =g_total/count;
            png_byte b_avg =b_total/count;

            // now we just need to set all pixels in block to avg colour

            for(int dy=0;dy<factor && y+dy<height;dy++){
                png_bytep row =row_pointers[y+dy];
                for(int dx=0;dx<factor && x+dx<width;dx++){
                    png_bytep pixel =&(row[(x+dx)*3]);
                    pixel[0]=r_avg;
                    pixel[1]=g_avg;
                    pixel[2]=b_avg;
                }
            }
        }
    }
}

void write_image(FILE * fp,png_bytep * row_pointers,int width, int height){
    png_structp png= png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
    if(!png){
        err="png_create_write_struct failed\n";
        fclose(fp);
        return ;
    }
    // creating png info struct
    png_infop info= png_create_info_struct(png);
    if(!info){
        err="png_create_info_struct failed\n";
        png_destroy_read_struct(&png,(png_infopp)NULL,(png_infopp)NULL);
        fclose(fp);
        return ;
    }

    png_init_io(png,fp);
    png_set_IHDR(png,info,width,height,8,PNG_COLOR_TYPE_RGB,PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png,info);
    png_write_image(png,row_pointers);
    png_write_end(png, NULL);

    for(int i=0;i<height;i++){
        free(row_pointers[i]);
    }
    free(row_pointers);

    png_destroy_write_struct(&png,&info);
}

png_bytep * read_image(FILE * fp,int * width,int * height){
    // check file signature
    png_byte signature[8];
    fread(signature,1,8,fp);
    if(png_sig_cmp(signature,0,8)){
        err="File signature is not correct\n";
        return NULL;
    }
    // creating a png read struct
    png_structp png= png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
    if(!png){
        err="png_create_read_struct failed\n";
        fclose(fp);
        return NULL;
    }
    // creating png info struct
    png_infop info= png_create_info_struct(png);
    if(!info){
        err="png_create_info_struct failed\n";
        png_destroy_read_struct(&png,(png_infopp)NULL,(png_infopp)NULL);
        fclose(fp);
        return NULL;
    }

    png_init_io(png,fp);
    png_set_sig_bytes(png,8);
    png_read_info(png,info);
    
    *width=png_get_image_width(png,info);
    *height=png_get_image_height(png,info);

    // Standardization to make png file handling easier

    png_set_palette_to_rgb(png);
    png_set_expand_gray_1_2_4_to_8(png);
    png_set_strip_16(png);
    png_read_update_info(png,info);

    png_bytep *row_pointers= (png_bytep *)malloc(sizeof(png_bytep)*(*height));
    for(int i=0;i<(*height);i++){
        row_pointers[i]=(png_byte *)malloc(png_get_rowbytes(png,info));
    }
    
    png_read_image(png,row_pointers);

    png_destroy_read_struct(&png,&info,NULL);

    return row_pointers;
}

int main(int argc, char const *argv[])
{
    if(argc<3){
        fprintf(stderr,"Usage ./pixelate <input_image_name.png>  <output_image_name.png> <pixelation_factor>(optional)\n");
        return 1;
    }
    const char * input=NULL, *output=NULL;
    input=argv[1];
    output=argv[2];
    if(argc>3){
        PIXELATION_FACTOR=atoi(argv[3]);
    }
    if(!correct_file_name(input)){
        fprintf(stderr,"Incorrect input file name\n");
        return 1;
    }
    if(!correct_file_name(output)){
        fprintf(stderr,"Incorrect output file name\n");
        return 1;
    }
    FILE *fp= fopen(input,"rb");
    if(!fp){
        fprintf(stderr,"File cannot be opened\n");
        return 1;
    }
    int width,height;
    png_bytep * row_pointers = read_image(fp,&width,&height);

    fclose(fp);
    if(err){
        fprintf(stderr,err);
        return 1;
    }

    pixalate(row_pointers,width,height,PIXELATION_FACTOR);

    fp=fopen(output,"wb");
    if(!fp){
        fprintf(stderr,"Output File cannot be opened\n");
        return 1;
    }

    write_image(fp,row_pointers,width,height);
    fclose(fp);
    if(err){
        fprintf(stderr,err);
        return 1;
    }

    printf("Image pixalated Successfully\n");

    
    return 0;
}
