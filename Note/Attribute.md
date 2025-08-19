# Attribute

```c++
UPROPERTY()
	FGameplayAttributeData Health;
```

FGameplayAttributeData：

```c++
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Attribute")
	float BaseValue;

	UPROPERTY(BlueprintReadOnly, Category = "Attribute")
	float CurrentValue;
```

基础值是最初设定的固定值，也就是起始值。例如，如果你从一级开始，你有 700 点生命值。这就是那个数值，对吧？

当前值就是此刻的数值。这是什么意思？在基础数值之上，你可能还拥有一些增益效果，对吧？ 例如，如果你获得了一个生命值增益，我们再加上 200 点。 那么基础数值仍将保持为 700 点。 但当前数值就是 900 点了，没错吧。

或者你获得一件新装备。这件装备会赋予你更多力量，从而为你带来更高的生命值。所以你的Health属性将再次提升。 当前数值也会随之提高，对吧？但你的基础值仍然是那个旧数值。所以当前值就是基础值加上你携带的任何增益装备效果，这些效果会改变这个数值。

当前值实际上就是计算生命值减少时所使用的值，对吧。 在计算生命值减少时，或者任何显示内容。，实际使用的将是当前值，对吧。

但基础值是指在没有任何增益效果或装备改变该数值时的原始值。别理解错了。对最大值感到困惑。