#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<assert.h>
#include<sys/types.h>
#include<sys/stat.h>

void int_to_bytes(uint32_t valor, uint8_t* destiny){
    int size= 4;
    for(int i = 0; i < size; i++){
        destiny[i] = valor >> i*8;
    }
}
void int_to_bytes_inverso(uint32_t valor, uint8_t* destiny){
    int size = 4;
    for(int i = 0; i < size; i++){
        destiny[i] = valor >> (size-i-1)*8;
    }
}

unsigned int bytes_to_int_inverso(uint8_t *buffer, unsigned int size){
    unsigned int valor = 0;
    for(int i = 0; i<size; i++){
        valor = valor | buffer[i] << (size-1-i)*8;
    }
    return valor;
}

int main(int argc, char* argv[]){
    
    assert(argc == 2 && "Not enough Files");
    
    FILE* file = fopen(argv[1], "rb");
    FILE* output = fopen("saida.png", "wb+");
    assert(file != NULL && "The file exists");

    uint8_t indentifier_buffer[8];
    fread(indentifier_buffer, sizeof(indentifier_buffer), 1, file);
    for(int i = 0; i < sizeof(indentifier_buffer); i++){
        printf("%c ", indentifier_buffer[i]);
    }
    printf("\n");
    fwrite(indentifier_buffer, sizeof(indentifier_buffer), 1, output);

    uint8_t size_buffer[4];
    uint8_t type_buffer[4];
    uint8_t *data_buffer = calloc(1, 1);
    uint8_t key_buffer[4];
    unsigned int valor; // this will be used to storing char[4] into uint32
    for(int chunknum = 0; chunknum < 1; chunknum++){
        
        //SIZE READING
        fread(size_buffer, sizeof(size_buffer), 1, file);

        printf("SIZE -> ");
        for(int i = 0; i < sizeof(size_buffer); i++) printf("%u ", size_buffer[i]);
        printf("\n");

        valor = bytes_to_int_inverso(size_buffer, sizeof(size_buffer));
        printf("%u\n", valor);
        fwrite(size_buffer,sizeof(size_buffer), 1, output);
        
        //TYPE -> SIGNATURE READING
        fread(type_buffer, sizeof(type_buffer), 1, file);
        for(int i = 0; i < sizeof(type_buffer); i++){
            printf("%c ", type_buffer[i]);
        }
        printf("\n");
        fwrite(type_buffer, sizeof(type_buffer), 1, output);
        
        //DATA READING
        data_buffer = reallocarray(data_buffer, valor, sizeof(int8_t));
        fread(data_buffer, valor, 1, file);
        for(int i = 0; i < valor; i++){
            printf("%u ", data_buffer[i]);
        }
        printf("\n");
        fwrite(data_buffer, valor, 1, output);
        
        //CRC READING, YEAH I CALLED IT KEY
        fread(key_buffer, sizeof(key_buffer), 1, file);
        for(int i = 0; i < sizeof(key_buffer); i++){
            printf("%u ", key_buffer[i]);
        }
        printf("\n");
        fwrite(key_buffer, sizeof(key_buffer), 1, output);
        
        printf("END OF CHUNK %u \n", chunknum);
    }
    //Inject info here
    //uint8_t injected_data[] = "   Frederico e muito foda; Nice Cock";
    //int_to_bytes_inverso(sizeof(injected_data), size_buffer);
    
    FILE* target = fopen("image2.png", "rb");
    
    //SIZE
    struct stat stats;
    stat("image2.png", &stats);
    uint32_t image_size_int = stats.st_size;
    int_to_bytes_inverso(image_size_int, size_buffer);
    //DATA
    uint8_t *injected_data = calloc(image_size_int, sizeof(int8_t));
    fread(injected_data, image_size_int, 1, target);
    //type
    uint8_t injected_type[4] = {'f', 'f', 'F', 'f'};
    //use the previous crc
    //
    fwrite(size_buffer, sizeof(size_buffer), 1, output);

    fwrite(injected_type, sizeof(injected_type), 1, output);
    fwrite(injected_data, image_size_int, 1, output);

    //CRC
    fwrite(key_buffer, sizeof(key_buffer), 1, output);

    //Stop injecting info here
    uint8_t rest_buffer[1];
    while(!feof(file)){
        fread(rest_buffer, sizeof(rest_buffer), 1, file);
        fwrite(rest_buffer, sizeof(rest_buffer), 1, output);
    }
    
    //free(injected_data);
    free(data_buffer);
    fclose(file);
    fclose(output);
    return 0;
}
