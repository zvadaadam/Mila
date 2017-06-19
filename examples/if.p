program ifTest;

	var foo;

begin
	if 5 > 3 then
		begin
			write 1;
			foo := 40+2;
			write foo;
		end;
	else 
		begin
			foo := 20+2;
			write foo;
			write 2;
		end;
end.
