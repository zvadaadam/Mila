program arrayTest;
	var arr : array [0 .. 42];
	var i : integer;
begin
	for i := 0 to 41 do begin
		arr[i] := i;
	end;

	for i := 0 to 41 do begin
		write arr[i];
	end;
end.
