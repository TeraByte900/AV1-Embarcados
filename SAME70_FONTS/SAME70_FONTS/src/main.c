/*
 * main.c
 *
 * Created: 05/03/2019 18:00:58
 *  Author: eduardo
 */ 

#include <asf.h>
#include "tfont.h"
#include "sourcecodepro_28.h"
#include "calibri_36.h"
#include "arial_72.h"

#define BUT1_PIO           PIOD
#define BUT1_PIO_ID        16
#define BUT1_PIO_IDX       28u
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define BUT2_PIO           PIOC
#define BUT2_PIO_ID        12
#define BUT2_PIO_IDX       31u
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

#define BUT3_PIO           PIOA
#define BUT3_PIO_ID        10
#define BUT3_PIO_IDX       19u
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX)

volatile int radius = 0.65/2;
volatile int counter = 0;


struct ili9488_opt_t g_ili9488_display_opt;

void configure_lcd(void){
	/* Initialize display parameter */
	g_ili9488_display_opt.ul_width = ILI9488_LCD_WIDTH;
	g_ili9488_display_opt.ul_height = ILI9488_LCD_HEIGHT;
	g_ili9488_display_opt.foreground_color = COLOR_CONVERT(COLOR_WHITE);
	g_ili9488_display_opt.background_color = COLOR_CONVERT(COLOR_WHITE);

	/* Initialize LCD */
	ili9488_init(&g_ili9488_display_opt);
	ili9488_draw_filled_rectangle(0, 0, ILI9488_LCD_WIDTH-1, ILI9488_LCD_HEIGHT-1);
	
}


void font_draw_text(tFont *font, const char *text, int x, int y, int spacing) {
	char *p = text;
	while(*p != NULL) {
		char letter = *p;
		int letter_offset = letter - font->start_char;
		if(letter <= font->end_char) {
			tChar *current_char = font->chars + letter_offset;
			ili9488_draw_pixmap(x, y, current_char->image->width, current_char->image->height, current_char->image->data);
			x += current_char->image->width + spacing;
		}
		p++;
	}	
}

void BUT1_Handler(){
	
}

void BUT2_Handler(){
	
}

void BUT3_Handler(){
	
}

void But_init(){
	
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	
	pio_set_input(BUT1_PIO,BUT1_PIO_IDX_MASK,PIO_DEFAULT);
	pio_set_input(BUT2_PIO,BUT2_PIO_IDX_MASK,PIO_DEFAULT);
	pio_set_input(BUT3_PIO,BUT3_PIO_IDX_MASK,PIO_DEFAULT);
	
	pio_pull_up(BUT1_PIO, BUT1_PIO_IDX_MASK, 1);
	pio_pull_up(BUT2_PIO, BUT2_PIO_IDX_MASK, 1);
	pio_pull_up(BUT3_PIO, BUT3_PIO_IDX_MASK, 1);
	
	pio_handler_set(BUT1_PIO, BUT1_PIO_ID, BUT1_PIO_IDX_MASK, PIO_IT_EDGE, BUT1_Handler);
	pio_handler_set(BUT2_PIO, BUT2_PIO_ID, BUT2_PIO_IDX_MASK, PIO_IT_EDGE, BUT2_Handler);
	pio_handler_set(BUT3_PIO, BUT3_PIO_ID, BUT3_PIO_IDX_MASK, PIO_IT_EDGE, BUT3_Handler);
	
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_EnableIRQ(BUT3_PIO_ID);
		
	NVIC_SetPriority(BUT1_PIO_ID, 1);
	NVIC_SetPriority(BUT2_PIO_ID, 1);
	NVIC_SetPriority(BUT3_PIO_ID, 1);
}


int main(void) {
	board_init();
	sysclk_init();	
	configure_lcd();
	But_init();
	
	char speed_buffer[32];
	
	font_draw_text(&sourcecodepro_28, "BIKE", 50, 50, 1);
	font_draw_text(&sourcecodepro_28, "VELOCIDADE", 50, 80, 1);
	font_draw_text(&sourcecodepro_28, speed_buffer, 50, 100, 1);
	while(1) {
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}