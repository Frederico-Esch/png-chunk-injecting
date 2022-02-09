#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<assert.h>
#include<string.h>

uint32_t bytes_to_int(uint8_t *buffer){
    uint32_t valor = 0;
    for(int i = 0; i < 4; i ++){
        valor = valor | buffer[i] << (3-i)*8;
    }
    return valor;
}

int main(int argc, char* argv[]){
    assert(argc == 2 && "Uma imagem não foi fornecida");
    FILE* file = fopen(argv[1], "rb");
    assert(file != NULL && "Não foi possivel abrir a imagem");
    
    uint8_t signature[8];
    uint8_t size_buffer[4];
    uint8_t type_buffer[4];
    uint32_t size_of_chunk;
    uint8_t* data_buffer;
    uint8_t crc_buffer[4];

    uint8_t my_type[] = {'f', 'f', 'F', 'f'};

    int found = 0;
        
    fread(signature, sizeof(signature), 1, file);

    while(!found && !feof(file)){
        fread(size_buffer, sizeof(size_buffer), 1, file);
        size_of_chunk = bytes_to_int(size_buffer);
        fread(type_buffer, sizeof(type_buffer), 1, file);
        if(!memcmp(type_buffer, my_type, 4)){
            found = 1;
        }else{
            fseek(file, size_of_chunk+4, SEEK_CUR);
        }
    }
    
    if(!found){
        printf("NAO ENCONTRADO");
        return 1;
    }
    
    data_buffer = calloc(size_of_chunk, sizeof(uint8_t));
    fread(data_buffer, size_of_chunk, 1, file);

    FILE* target = fopen("target.png", "wb+");

    fwrite(data_buffer, size_of_chunk, 1, target);
    printf("%u\n", size_of_chunk);
    
    free(data_buffer);
    fclose(file);
    fclose(target);
    return 0;
}
