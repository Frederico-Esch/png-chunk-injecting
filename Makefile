test1: origin.c
	gcc origin.c -o output
	./output image1.png

test2: origin.c
	gcc origin.c -o output
	./output image2.png

rec: reconstruct.c saida.png
	gcc reconstruct.c -o output
	./output saida.png

clean:
	rm ./output
	rm ./a.out
