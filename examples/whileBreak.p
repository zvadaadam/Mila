program whileBreak;

	var foo;
begin
	foo := 0;
	while foo < 10 do begin
		foo := foo + 1;
		write foo;
		if foo > 5 then break;
	end;
end.
