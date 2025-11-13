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



## BlueprintType和Blueprintable的区别

在Unreal Engine（UE4和UE5）中，**BlueprintType**和**Blueprintable**是用于C++类和结构体的两个重要宏，它们在蓝图系统中的作用有所不同。

**BlueprintType**主要用于使C++类或结构体能够在蓝图中作为变量使用。也就是说，使用BlueprintType修饰的类或结构体可以在其他蓝图中被引用和操作。例如：

```c++
USTRUCT(BlueprintType)

struct FMyStruct

{

GENERATED_BODY()

UPROPERTY(EditAnywhere, BlueprintReadWrite)

int32 MyInt;

};
```



在这个例子中，FMyStruct结构体被声明为BlueprintType，这意味着它可以在蓝图中作为变量使用。

**Blueprintable**则是用于使C++类能够被蓝图继承和扩展。使用Blueprintable修饰的类可以在蓝图编辑器中被继承，并且可以在蓝图中添加新的功能和逻辑。例如：

```c++
UCLASS(Blueprintable)

class MYPROJECT_API AMyActor : public AActor

{

GENERATED_BODY()

public:

AMyActor();

};
```



在这个例子中，AMyActor类被声明为Blueprintable，这意味着它可以在蓝图中被继承，并且可以在蓝图中添加新的属性和方法。

总结来说，**BlueprintType**用于使类或结构体在蓝图中作为变量使用，而**Blueprintable**用于使类能够被蓝图继承和扩展。它们在蓝图系统中的应用场景不同，但都是为了增强C++代码与蓝图系统的交互性和灵活性。



